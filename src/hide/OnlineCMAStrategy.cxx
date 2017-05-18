#include "cmaes.h"

#include "OnlineCMAStrategy.hxx"

using namespace libcmaes;

OnlineCMAStrategy::OnlineCMAStrategy()
{
	candidates_uh_set = false;
	nfcalls = 0;
	genotype = 0;
}

OnlineCMAStrategy::OnlineCMAStrategy(FitFunc &func,
		CMAParameters<> &parameters)
:CMAStrategy<CovarianceUpdate>(func,parameters)
{
	candidates_uh_set = false;
	nfcalls = 0;
	genotype = 0;
}

void OnlineCMAStrategy::init(int inEvaluationTime)
{
	evaluationTime = inEvaluationTime;
	//init the parameters of the evo engine
	candidates = ask();
	popSize = candidates.cols();
	phenotypes = dMat(evaluationTime,popSize);
	phenotypes_uh = dMat(evaluationTime,popSize);

	nvcandidates.clear();
}

dMat OnlineCMAStrategy::ask()
{
	return CMAStrategy<CovarianceUpdate>::ask();
}

void OnlineCMAStrategy::eval(const dMat &candidates,
		const dMat &phenocandidates)
{
	// custom eval.
	for (int r=0;r<candidates.cols();r++)
	{
		_solutions.get_candidate(r).set_x(candidates.col(r));
		_solutions.get_candidate(r).set_fvalue(_func(phenocandidates.col(r).data(),candidates.rows()));
	}

	nfcalls = candidates.cols();
	// evaluation step of uncertainty handling scheme.

	CMAStrategy<CovarianceUpdate>::select_candidates_uh(candidates,dMat(0,0),candidates_uh);
	candidates_uh_set = true;
}

void OnlineCMAStrategy::end_eval(std::vector<RankedCandidate> nvcandidates)
{
	candidates_uh_set = false;
	CMAStrategy<CovarianceUpdate>::set_candidates_uh(nvcandidates);
	update_fevals(nfcalls);
}

dMat OnlineCMAStrategy::get_candidates_uh()
{
	return candidates_uh;
}

void OnlineCMAStrategy::increase_nfcalls()
{
	nfcalls ++;
}

bool OnlineCMAStrategy::isset_candidates_uh()
{
	return candidates_uh_set;
}

void OnlineCMAStrategy::tell()
{
	CMAStrategy<CovarianceUpdate>::tell();
}

bool OnlineCMAStrategy::stop()
{
	return CMAStrategy<CovarianceUpdate>::stop();
}

void OnlineCMAStrategy::setFitness(int timeStep, double fitness)
{
	//std::cout << timeStep%evaluationTime << " " << fitness << std::endl;
	if(genotype < popSize)
	{
		phenotypes((timeStep%evaluationTime),genotype) = fitness;
	}
	else
	{
		phenotypes_uh((timeStep%evaluationTime),(genotype-popSize)) = fitness;
	}
}

void OnlineCMAStrategy::completeEvaluation()
{
	genotype++;

	//if we are at the end of the regular evaluation, perform the evals
	//eval will set the candidates_uh
	if ((genotype == popSize) && (candidates_uh_set == false))
	{
		eval(candidates,phenotypes);
		nvcandidates.clear();
	}

	//if we are at the end of the re-evaluations we have to  place the result
	//and get the new generation
	if ((genotype == popSize + candidates_uh.cols()) && (candidates_uh_set == true))
	{
		for (int r=0;r<candidates.cols();r++)
		{
			//place the result of  the re-evaluation of re-evaluated genotypes
			if (r < candidates_uh.cols())
			{
				//double nfvalue = computeFitness(phenotypes_uh.col(r).data(),phenotypes_uh.rows());
				double nfvalue = _func(phenotypes_uh.col(r).data(),phenotypes_uh.rows());
				Candidate tmp(nfvalue,dVec(candidates.col(r)));
				nvcandidates.emplace_back(nfvalue,tmp,r);
				increase_nfcalls();
			}
			//place the results previously obtained for the other genotypes
			else
			{	
				//double nfvalue = computeFitness(phenotypes.col(r).data(),phenotypes_uh.rows());
				double nfvalue = _func(phenotypes.col(r).data(),phenotypes_uh.rows());
				Candidate tmp(nfvalue,dVec(candidates.col(r)));
				nvcandidates.emplace_back(nfvalue,tmp,r);
			}
		}

		//finish the evaluation
		end_eval(nvcandidates);
		tell();
		inc_iter(); 

		//if we have not finish prepare the next generation
		if(!stop())
		{
			candidates = ask();
			genotype = 0;
		}
	}
}

std::vector<double> OnlineCMAStrategy::getParams()
{
	std::vector<double> params;
	if (stop() == true)
	{
		dVec tmp = get_solutions().best_candidate().get_x_dvec();
		for(int i = 0 ; i < tmp.cols() ; i++)
		{
			params.push_back(tmp(i));
		}
	}
	else
	{
		if (genotype < popSize)
		{
			params.clear();
			for(int i = 0 ; i < candidates.rows() ; i++)
			{
				params.push_back(candidates(i,genotype));
			}
		}
		else
		{
			params.clear();
			for(int i = 0 ; i < candidates.rows() ; i++)
			{
				params.push_back(candidates(i,genotype-popSize));
			}
		}
	}
	return params;
}
