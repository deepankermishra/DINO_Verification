// -----------------------------------------------------
//
//
//     Created by : Deepanker Mishra and Garvit Jain
//
//
//-------------------------------------------------------

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/CFG.h"
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

using namespace llvm;

#define DEBUG_TYPE "liveness"


const char * DINOTaskBoundaryFunctionName = "__dino_task_boundary";


bool findin(std::vector<std::string> v,std::string s){
	for(int i=0;i<v.size();i++){
		if(v[i]==s){
			return true;
		}
	}
	return false;
}


namespace {

	bool isCallTo(llvm::Instruction &I, StringRef fname) {
		llvm::CallInst *call = llvm::dyn_cast<llvm::CallInst>(&I);
		if (!call){
			return false;
		}
		llvm::Value *func = call->getCalledValue();
		if (!func){
			return false;
		}
		int at = func->getName().find(fname);
		if(at==std::string::npos){
			return false;
		}
		return true;
	}

	bool isTaskBoundaryInstruction(llvm::Instruction &I){
		return isCallTo(I, DINOTaskBoundaryFunctionName);
	}


	struct liveness : public FunctionPass {
		static char ID;
		liveness() : FunctionPass(ID) {}
		virtual bool runOnFunction(Function &F) {
			int ninst=-1;
			for (Function::arg_iterator AI = F.arg_begin(), AE = F.arg_end();AI != AE; ++AI)
				if (!AI->hasName() && !AI->getType()->isVoidTy())
					AI->setName("arg");

			for (BasicBlock &BB : F){
				if (!BB.hasName())
					BB.setName("bb");
				for (Instruction &I : BB){ 
					ninst++;
					if (!I.hasName() && !I.getType()->isVoidTy())
						I.setName("tmp");
				}
			}

			std::map<std::string,std::vector<std::string> > in_of_block;
			std::map<int,std::set<std::string> > user_passed_variables;
			std::map<int,std::vector<std::string> > in_of_task_boundary;
			std::map<std::string,std::vector<std::string> > prev_in_of_block;
			bool changed = 1;
			int taskid;
		
			while(changed){
				taskid = 0;
				changed = 0;
				for(Function::iterator b = --F.end();;--b){
					std::vector<std::string> in;
					std::vector<std::string> out;
					bool start=1;
					BasicBlock *BB = &*b;
					bool loadInTemp = 0;
					std::string loadInTempName = "";
					for (BasicBlock::iterator I = --b->end();; --I)
					{   
						if(start){
							for (succ_iterator PI = succ_begin(BB), E = succ_end(BB); PI != E; ++PI){
								BasicBlock *Suc = *PI;
								int w;
								std::vector<std::string> X = in_of_block[Suc->getName()];
								for(w=0;w<X.size();w++)
								{
									if (std::find(out.begin(), out.end(), X[w])==out.end())
										out.push_back(X[w]);
								}
							}
							start=0;
						}
						else{
							out = in;
						}


						//def:
						User::op_iterator OI, OE;
						if(I->hasName())
						{ 
							out.erase(std::remove(out.begin(), out.end(), I->getName()), out.end());
						}
						
						//use in optimized (optional):
						// if (PHINode* phi_insn = dyn_cast<PHINode>(&*I)) {
	     //                    for (int ind = 0; ind < phi_insn->getNumIncomingValues(); ind++) {
	     //                        Value* val = phi_insn->getIncomingValue(ind);
	     //                        if (isa<Instruction>(val) || isa<Argument>(val)) {
		    //                         if (std::find(out.begin(), out.end(), val->getName())==out.end())
						// 				out.push_back(val->getName());    
	     //                        }
	     //                    }
						// }
						if(!isTaskBoundaryInstruction(*I)){
							for (OI = I->op_begin(), OE = I->op_end(); OI != OE; ++OI)
							{
								Value *val = *OI;
								if(isa<Instruction>(val) || isa<Argument>(val))
								{
									//val is used by insn
									if(isa<LoadInst>(I))
									{	
										if(loadInTemp){
											loadInTemp = 0;
											std::string star = "*";
											if(I->getName() == loadInTempName){
												out.push_back(star.append(I->getOperand(0)->getName().str()));
											}
										}
										int tmpload = I->getOperand(0)->getName().find("tmp");
										if(tmpload!=std::string::npos){
											loadInTemp = 1;
											loadInTempName = I->getOperand(0)->getName();
										}
									}
									// if(isa<Argument>(val))
									// 	errs() << "arg: "<<val->getName() << "\n";
									// else
									// 	errs() << "inst: "<<val->getName() << "\n";
									if (std::find(out.begin(), out.end(), val->getName())==out.end())
										out.push_back(val->getName());
									if(isa<StoreInst>(I))
									{
										out.erase(std::remove(out.begin(), out.end(), I->getOperand(1)->getName()), out.end());
									}
								}
							}
						}
						in=out;
						if(isTaskBoundaryInstruction(*I))
						{
							in_of_task_boundary[taskid]=in;
							for (auto argi = I->op_begin(), arge = I->op_end(); argi != arge; ++argi)
							{
								Value *argval = *argi;
								if(isa<Instruction>(argval) || isa<Argument>(argval))
								{
									user_passed_variables[taskid].insert(argval->getName().str());
									//errs()<<argval->getName()<<"\n";
								}
							}
							taskid++;
						}

						if(I==b->begin())
						{
							in_of_block[BB->getName()]=in;
							break;
						}
					}
					if(b==F.begin()){
						break;
					}
				}
				if(prev_in_of_block.empty() && !in_of_block.empty())
				{
					changed = 1;
					prev_in_of_block = in_of_block;
				}
				for(auto it = prev_in_of_block.begin();it != prev_in_of_block.end();it++){
					if(in_of_block.size() == prev_in_of_block.size())
					{
						for(int i=0;i<it->second.size();i++)
						{
							if(!findin(in_of_block[it->first],it->second[i]) ){
								changed = 1;
								prev_in_of_block = in_of_block;
								break;
							}
						}
					}
					else{
						changed = 1;
						prev_in_of_block = in_of_block;
						break;
					}

				}
			  //errs()<<"--------------------------\n";
			}
			// for(auto it = prev_in_of_block.begin();it != prev_in_of_block.end();it++){
			//   errs()<<"Block Name: "<<it->first<<"\n";
			//   for(int i=0;i<it->second.size();i++){
			//     errs()<<it->second[i]<<" ";
			//   }
			//   errs()<<"--------------------------\n";
			// }

			//Output task boundaries checkpoint variables
			for(int i=0;i<taskid;i++)
			{
				errs() <<"\n" << i << ": ";
				for(int j=0;j<in_of_task_boundary[i].size();j++)
				{
					errs() << in_of_task_boundary[i][j]<<", ";
				}
			}

			// errs() << "****** User passed **********"<<"\n";
			// for(int i=0;i<taskid;i++)
			// {
			// 	errs() <<"\n" << i << ": ";
			// 	for(auto j=user_passed_variables[i].begin();j!=user_passed_variables[i].end();j++)
			// 	{
			// 		errs() << *j<<", ";
			// 	}
			// }

			//Verifier code:
			for(int i=0;i<taskid;i++)
			{
				for(int j=0;j<in_of_task_boundary[i].size();j++)
				{
					if(user_passed_variables[i].find(in_of_task_boundary[i][j])==user_passed_variables[i].end()){
						errs()<<"\n!! WRONG !! Error at task boundary "<<i<<", missing "<<in_of_task_boundary[i][j];
						i = taskid;
						break;
					}
				}
			}
			errs()<<"\n";


		}
	};
}
char liveness::ID = 0;
static RegisterPass<liveness> X("liveness", "verification of dino");