#ifndef __OnlineCMAStrategy_hxx
#define __OnlineCMAStrategy_hxx

#include "cmaes.h"
#include <vector>

namespace libcmaes
{

class OnlineCMAStrategy : public CMAStrategy<CovarianceUpdate>
{
public:
  OnlineCMAStrategy();

  OnlineCMAStrategy(FitFunc &func,CMAParameters<> &parameters);

  ~OnlineCMAStrategy(){}

	void init(int inEvaluationTime);

  dMat ask();

  void eval(const dMat &candidates,const dMat &phenocandidates);

	void end_eval(std::vector<RankedCandidate> nvcandidates);

	dMat get_candidates_uh();

	void increase_nfcalls();

	bool isset_candidates_uh();
  
  void tell();

  bool stop();

	void setFitness(int timeStep, double fitness);

	void completeEvaluation();

	std::vector<double> getParams();

protected:
	dMat candidates;
	dMat candidates_uh;
  bool candidates_uh_set;
	int nfcalls;

	int genotype;
	int popSize;
	int evaluationTime;
	
	dMat phenotypes;
	dMat phenotypes_uh;

  std::vector<RankedCandidate> nvcandidates;
};
}

#endif
