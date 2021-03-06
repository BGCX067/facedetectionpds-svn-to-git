#include "svm.h"
#include <fstream>
#include <vector>
#include "stdio.h"
#include "cv.h"
#include "highgui.h"

#define FACE 2429
#define NON_FACE 4548

using namespace std;

class Unit
{
public:
	
	double* value;
	bool face;
	int dim;

	Unit(int dimTmp)
	{
		value = (double*)malloc(sizeof(double)*dimTmp);
		dim = dimTmp;
	}

	~Unit()
	{
		free(value);
	}
};

vector<Unit*> base;

fstream saida;

IplImage *mask;

void adicionarImagem(int face, std::string arquivo)
{
	int var = 0;
	IplImage *readImage;	
	
	readImage = cvLoadImage(arquivo.c_str(),0);
	IplImage* imagem = cvCreateImage( cvGetSize(readImage), IPL_DEPTH_8U, 1 );
	
	cvEqualizeHist(readImage, imagem);
		
	if (face == 1)
	{
		saida << "1 ";
	}
	else
	{
		saida << "-1 ";
	}

	for (int i = 0; i < imagem->height; i++)
	{
		for (int j = 0; j < imagem->width; j++)
		{
			var++;
			double valor = 0;

			CvScalar scalarValor = cvGet2D(imagem, i, j);
			CvScalar scalarMask = cvGet2D(mask, i, j);
			/*double valor = 0.257*s.val[2] + 0.504*s.val[1] + 0.098*s.val[0] + 16;	*/
					
			//if (scalarMask.val[0] != 0)
			//{
				valor = scalarValor.val[0];
			//}

			if (valor != 0)
			{
				saida << var << ":" << valor << " ";
			}
		}
	}

	/*cvNamedWindow("Normal", CV_WINDOW_AUTOSIZE);
	cvResizeWindow("Normal", readImage->width, readImage->height);
	cvShowImage("Normal", readImage);

	cvNamedWindow("Modificada", CV_WINDOW_AUTOSIZE);
	cvResizeWindow("Modificada", imagem->width, imagem->height);
	cvShowImage("Modificada", imagem);

	int key = cvWaitKey(0);

	cvDestroyWindow("Normal");
	cvDestroyWindow("Modificada");*/

	cvReleaseImage(&imagem);
	cvReleaseImage(&readImage);

	saida << endl;
}

void treinamento()
{
	mask = cvLoadImage("c:/ufpe/pds/projeto/facedetectionpds/mask.pgm",0);

	std::string nameImagem = "c:/faces/train/face/face (";
	std::string formatoImagem = ").pgm";
	
	saida.open("train", ios::out);
	
	for (int i = 0; i < FACE; i++)
	{
		std::string tmp;
		char valorIntermediario[33];
		
		itoa(i, valorIntermediario, 10);

		tmp.append(nameImagem);
		tmp.append(valorIntermediario);
		tmp.append(formatoImagem);	
		adicionarImagem(1, tmp);	
	}

	nameImagem = "c:/faces/train/non-face/non-face (";

	for (int i = 0; i < NON_FACE; i++)
	{
		std::string tmp;
		char valorIntermediario[33];
		
		itoa(i, valorIntermediario, 10);

		tmp.append(nameImagem);
		tmp.append(valorIntermediario);
		tmp.append(formatoImagem);		

		adicionarImagem(0, tmp);
	}

	saida.close();

	saida.open("test", ios::out);

	nameImagem = "c:/faces/test/face/cmu (";

	for (int i = 0; i < 472; i++)
	{
		std::string tmp;
		char valorIntermediario[33];
		
		itoa(i, valorIntermediario, 10);

		tmp.append(nameImagem);
		tmp.append(valorIntermediario);
		tmp.append(formatoImagem);		

		adicionarImagem(1, tmp);
	}

	nameImagem = "c:/faces/test/non-face/cmu_";
	formatoImagem = ".pgm";

	for (int i = 0; i < 23573; i++)
	{
		std::string tmp;
		char valorIntermediario[33];

		itoa(i, valorIntermediario, 10);	
		
		tmp.append(nameImagem);

		if (i < 10)
		{
			tmp.append("000");
		}
		else if (i < 100)
		{
			tmp.append("00");
		}
		else if (i < 1000)
		{
			tmp.append("0");
		}

		tmp.append(valorIntermediario);
		tmp.append(formatoImagem);		

		adicionarImagem(0, tmp);
	}

	cvReleaseImage(&mask);

	saida.close();

}

void initBase()
{
	//
	//int samples = 6977;
	//int dim = 361;

	////base = (double**) malloc(sizeof(double*)*samples);

	//for (int i = 0; i < samples; i++)
	//{
	//	base[i] = (double*) malloc(sizeof(double)*dim);
	//}	


}

void readBase()
{
	std::string nameImagem = "c:/faces/train/face/face (";
	std::string formatoImagem = ").pgm";
	
	for (int i = 0; i < FACE; i++)
	{
		std::string tmp;
		char valorIntermediario[33];
		
		itoa(i, valorIntermediario, 10);

		tmp.append(nameImagem);
		tmp.append(valorIntermediario);
		tmp.append(formatoImagem);

		IplImage *imagem = cvLoadImage(tmp.c_str(),0);

		Unit* tmpUnit = new Unit( (imagem->height) * (imagem->width) );

		tmpUnit->face = true;		
				
		for (int k = 0; k < imagem->height; k++)
		{
			for (int j = 0; j < imagem->width; j++)
			{
				CvScalar scalarValor = cvGet2D(imagem, k, j);
				tmpUnit->value[k*imagem->width + j] = scalarValor.val[0];				
			}
		}

		base.push_back(tmpUnit);
	}
	

	nameImagem = "c:/faces/train/non-face/non-face (";

	for (int i = 0; i < NON_FACE; i++)
	{
		std::string tmp;
		char valorIntermediario[33];
		
		itoa(i, valorIntermediario, 10);

		tmp.append(nameImagem);
		tmp.append(valorIntermediario);
		tmp.append(formatoImagem);		

		IplImage *imagem = cvLoadImage(tmp.c_str(),0);

		Unit* tmpUnit = new Unit( (imagem->height) * (imagem->width) );

		tmpUnit->face = false;
		
		for (int k = 0; k < imagem->height; k++)
		{
			for (int j = 0; j < imagem->width; j++)
			{
				CvScalar scalarValor = cvGet2D(imagem, k, j);	
				tmpUnit->value[k*imagem->width + j] = scalarValor.val[0];				
			}
		}

		base.push_back(tmpUnit);
	}
}

double innerProduct(Unit* m, Unit* n)
{
	double tmp = 0;

	for (int i = 0; i < (m->dim); i++)
	{
		tmp += m->value[i]*n->value[i];
	}

	return tmp;
}

void computeMutualInformation(vector<double> vectorF)
{
	double faceProbability = FACE / (FACE + NON_FACE);
	double non_faceProbability = NON_FACE / (FACE + NON_FACE);
	for(int i = 0; i < vectorF.size(); i++)
	{
		//vectorF.
	}

	
	//double jointHistogram_Face = ;
}

Unit* unitByScalar(Unit* unit, double scalar)
{
    Unit* result = new Unit(unit->dim);
    for(int i = 0; i < result->dim; i++)
    {
        result->value[i] = result->value[i] * scalar;
    }
    return result;
}

void subtract(Unit* x, Unit* base)
{
    for(int i = 0; i < x->dim; i++)
    {
        x->value[i] = x->value[i] - base->value[i];
    }
}

void makeOrthogonal(Unit* x, Unit* e)
{
    double innerProductTmp = innerProduct(x, e);
    Unit* e_temp = unitByScalar(e, innerProductTmp);

    subtract(x, e_temp);
}

double** iss(int samples, int dim, int k)
{
	for (int i = 0; i < k; i++)
	{
		for (int m = 0; m < samples; m++)	
		{
			vector<double> vectorF;

			for (int n = 0; n < samples; n++)
			{
				double F = innerProduct(base.at(m), base.at(n));
				vectorF.push_back(F);				
			}

			//computeMutualInformation();
			//Proc. Estimate joint probability P(Y, F(m, •))
			//Proc. Compute I(m) = I(Y, F(m, •))
			
		}

		/*If I(j) > I(m), ∀m Then gi = xj /||xj||
		Remove xj and yj from X and Y respectively
		N = N –1
		for m = 1 to N do
		xm = xm – < xm, gi >gi
		End for*/
	}

	return 0;
}

int main()
{	
	printf("%s\n", "begin readBase");

	readBase();

	printf("%s\n", "end readBase");

	printf("%s\n", "begin iss");

	iss(6977, 361, 10);

	printf("%s\n", "end iss");	

	//treinamento();


	/*imagem = cvLoadImage("c:/",1);
	
	cvNamedWindow("Teste", CV_WINDOW_AUTOSIZE);
	cvResizeWindow("Teste", imagem->width, imagem->height);
	cvShowImage("Teste", imagem);	

	int key = cvWaitKey(0);

	cvReleaseImage(&imagem);
	cvDestroyWindow("Teste"); 	*/

	return 0;
}