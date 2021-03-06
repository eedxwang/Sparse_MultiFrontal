#ifndef SPARSE_MF_HPP
#define SPARSE_MF_HPP

//Standard C++
#include <algorithm>
#include <fstream>                       
#include <iostream>                      
#include <set>      
#include <string>                                              
#include <vector> 

//External Dependencies   
//Eigen
#include <Eigen/Sparse>                                                                            
#include <Eigen/Dense>                         
//IML
#include "Eigen_IML_Vector.hpp"
#include "gmres.h" 

//SCOTCH
#include "scotch.h"

//Custom Dependencies 
#include "HODLR_Matrix.hpp" 
#include "eliminationTree.hpp"
#include "helperFunctions.hpp"
#include "HODLR_Matrix.hpp"
#include "extendAdd.hpp"

class sparseMF{

public:

  bool testResults;
  bool printResultInfo;
  sparseMF(Eigen::SparseMatrix<double> & inputSpMatrix);
  virtual ~sparseMF();
  Eigen_IML_Vector solve(const Eigen_IML_Vector & other);
  Eigen::MatrixXd LU_Solve(const Eigen::MatrixXd & inputRHS);
  Eigen::MatrixXd implicit_Solve(const Eigen::MatrixXd & inputRHS);
  Eigen::MatrixXd fast_Solve(const Eigen::MatrixXd & inputRHS);
  Eigen::MatrixXd iterative_Solve(const Eigen::MatrixXd & input_RHS, const int maxIterations, const double stop_tolerance,const double LR_Tolerance);

  void updateNumericalEntries(Eigen::SparseMatrix<double> newMatrix);

private:
  Eigen::SparseMatrix<double> reorderedMatrix;
  Eigen::SparseMatrix<double> reorderedMatrix_T;
  Eigen::SparseMatrix<double> L_Matrix;
  Eigen::SparseMatrix<double> U_Matrix;
  Eigen::VectorXd LU_Permutation;
  std::vector<int> permVector;
  eliminationTree* matrixElmTreePtr;
  std::vector<Eigen::Triplet<double,int> > L_TripletVec;
  std::vector<Eigen::Triplet<double,int> > U_TripletVec;
  int Sp_MatrixSize;
  int frontID;
  double averageLargeFrontSize;
  int numLargeFronts;

  int fast_MatrixSizeThresh;
  int fast_HODLR_LeafSize;
  int fast_BoundaryDepth;
  int fast_MaxRank;
  double fast_LR_Tol;
  double fast_MinPivot;

  
  bool symbolic_Factorized;
  bool LU_Factorized;
  bool implicit_Factorized;
  bool fast_Factorized;
  
  double matrixReorderingTime;
  double SCOTCH_ReorderingTime;
  double matrixGraphConversionTime;

  double symbolic_FactorizationTime;

  double implicit_FactorizationTime;
  double implicit_SolveTime;
  double implicit_TotalTime;
  double implicit_ExtendAddTime;
  double implicit_SymbolicFactorTime;

  double fast_FactorizationTime;
  double fast_SolveTime;
  double fast_TotalTime;
  double fast_ExtendAddTime;
  double fast_SymbolicFactorTime;

  double LU_FactorizationTime;
  double LU_SolveTime;
  double LU_TotalTime;
  double LU_ExtendAddTime;
  double LU_SymbolicFactorTime;
  double LU_AssemblyTime;


  /*************************************Reordering Related Functions*******************************/
  void reorderMatrix(Eigen::SparseMatrix<double> & inputSpMatrix);

  /***********************************Symbolic Factorization Functions****************************/
  void symbolic_Factorize();
  void symbolic_Factorize(eliminationTree::node* root);
  void updateNodeIdxWithChildrenFillins(eliminationTree::node* root,std::set<int> & idxSet);
  
  
  
  /**********************************Numerical Factorization Functions*****************************/
  Eigen::MatrixXd createPanelMatrix(eliminationTree::node* root);
  Eigen::SparseMatrix<double> createPanelMatrix_Sp(eliminationTree::node* root);
  Eigen::SparseMatrix<double> createPanelGraph(eliminationTree::node* root);
  void createPanelAndGraphMatrix(eliminationTree::node* root, Eigen::SparseMatrix<double> & panelMatrix, Eigen::SparseMatrix<double> & panelGraph);
                 

  void LU_FactorizeMatrix();
  void implicit_FactorizeMatrix();
  void fast_FactorizeMatrix();
  
  void LU_CreateFrontalAndUpdateMatrixFromNode(eliminationTree::node* root);
  void implicit_CreateFrontalAndUpdateMatrixFromNode(eliminationTree::node* root);
  void fast_CreateFrontalAndUpdateMatrixFromNode(eliminationTree::node* root);

  // Extend/Add
  void nodeExtendAddUpdate(eliminationTree::node* root,Eigen::MatrixXd & nodeFrontalMatrix, std::vector<int> & nodeMappingVector);
  void fast_NodeExtendAddUpdate(eliminationTree::node* root,HODLR_Matrix & panelHODLR,std::vector<int> & parentIdxVec);
  void fast_NodeExtendAddUpdate_Array(eliminationTree::node* root,HODLR_Matrix & panelHODLR,std::vector<int> & parentIdxVec);


  /*****************************************Solve Functions****************************************/

  //LU
  void assembleUFactor(const Eigen::MatrixXd & nodeMatrix_U, const Eigen::MatrixXd & update_U, const std::vector<int> & mappingVector);
  void assembleLFactor(const Eigen::MatrixXd & nodeMatrix_L, const Eigen::MatrixXd & update_L, const std::vector<int> & mappingVector);
  void assembleLUMatrix();

  //Implicit
  Eigen::MatrixXd implicit_UpwardPass(const Eigen::MatrixXd &inputRHS);
  void implicit_UpwardPass_Update(eliminationTree::node* root,Eigen::MatrixXd &modifiedRHS);

  Eigen::MatrixXd implicit_DownwardPass(const Eigen::MatrixXd & upwardPassRHS);
  void implicit_DownwardPass(eliminationTree::node* root,const Eigen::MatrixXd & upwardPassRHS,Eigen::MatrixXd & finalSoln);
  

  Eigen::MatrixXd fast_UpwardPass(const Eigen::MatrixXd &inputRHS);
  void fast_UpwardPass_Update(eliminationTree::node* root,Eigen::MatrixXd &modifiedRHS);

  Eigen::MatrixXd fast_DownwardPass(const Eigen::MatrixXd & upwardPassRHS);
  void fast_DownwardPass(eliminationTree::node* root,const Eigen::MatrixXd & upwardPassRHS,Eigen::MatrixXd & finalSoln);
  
  Eigen::MatrixXd fast_NodeToUpdateMultiply(eliminationTree::node* root,const Eigen::MatrixXd & RHS);
  Eigen::MatrixXd fast_UpdateToNodeMultiply(eliminationTree::node* root,const Eigen::MatrixXd & RHS);

  Eigen::MatrixXd fast_NodeSolve(eliminationTree::node* root,const Eigen::MatrixXd & RHS);
 
  Eigen::MatrixXd getRowBlkMatrix(const Eigen::MatrixXd & inputMatrix, const std::vector<int> & inputIndex);
  void setRowBlkMatrix(const Eigen::MatrixXd &srcMatrix, Eigen::MatrixXd &destMatrix, const std::vector<int> &destIndex);
  
  
  /*
  Eigen::MatrixXd oneStep_Iterate(const Eigen::MatrixXd & prevStep_result, const Eigen::MatrixXd & initSolveGuess, Eigen::MatrixXd & prevStep_Product);
  */


  /******************************************Test************************************************/
  void test_LU_Factorization();


  /**************************************General Extend Add*************************************/
  std::vector<int> extendIdxVec(std::vector<int> & childIdxVec, std::vector<int> & parentIdxVec);
};

#endif
