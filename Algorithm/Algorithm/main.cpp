#include "SvdPlusPlus.h"
#include "ParameterAdjust.h"
#include "ClusterSvdPPBase.h"

int main(int argc, char ** argv)
{  
	
	ParameterAdjust *tmp = new ParameterAdjust(943, 1682, "..\\movielens\\ml-100k\\u5.base", "..\\movielens\\ml-100k\\u5.test");
	tmp->SetAlpha(0.0035, 0.0050, 0.0001);
	tmp->SetBeta(0.12, 0.18, 0.01);
	tmp->SetDimension(40, 40, 1);
	tmp->SetGamma(0.000, 0.000, 0.001);
	tmp->SetDelta(0.0000, 0.0000, 0.0001);

	tmp->Adjust(3, 3);

	delete tmp;
	system("pause");
	return 0;
}