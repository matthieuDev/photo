#include <stdio.h>

#include "photo.h"
#include "Icosahedron.h"
#include "useful.h"
#include "bitmap.h"
#include "svdcmp.h"


#include <sstream>
#include <string> 
#include <vector>
#include <iostream>
#include <list>
#include<vector>
#include <algorithm>

using namespace std;


const string fileName = "data02_tile1";
const string pathName = "../data/" + fileName + "/data02/" ;

vector<Point3D> uniformLight;
int uniformSize = 0;
vector<Point3D> lights;
int width, height;

Point3D *normalEstimation ;

//denominateur image = dm
Point3D dmLight;
unsigned char * dmImage;
int dmPosition;

void output(Point3D p){ 
	cout << "Point3D("<< p.x  << ","  << p.y  << "," << p.w  << ");   " ;
};

bool compareiai(intensityAndimage  const &a, intensityAndimage  const &b){ return a.intensity < b.intensity; };

void getWidthHeight(){
	string newPathName = pathName + "image0001.bmp";
	char* s = (char*)(newPathName).c_str();

	readBMP((char*)s, width, height);
}

void loadIcosahedron(){
	string fileName = "../data/subdivided icosahedron.txt";



	ifstream input_file(fileName);

	if (!input_file.is_open())
	{
		std::cout << "Doesn't work" << std::endl;
	}

	int x, y, z;
	int size = 0;
	string line;
	while (getline(input_file, line))
	{
		std::istringstream iss(line);
		string s1, s2, s3;
		if (!(iss >> s1 >> s2 >> s3)) { break; } // error
		cout << "Point3D(" + s1 + " , " + s2 + " , " + s3 + "), \n";
		size++;
		// process pair (a,b)
	}
	/*while (input_file >> x >> y >> z)
	{
	cout << "Point3D(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "),";
	size++;
	}*/
	cout << std::to_string(size);
	input_file.close();
	cin >> size;

}

void loadLight(){
	string lightFile = pathName + "lightvec.txt";
	ifstream input_file(lightFile);

	double x, y, z;
	int test;

	while (input_file >> x >> y >> z)
	{
		lights.push_back(Point3D(x, y, z));
	}
	input_file.close();

}

void loadUniformLight(){
	string lightFile = pathName + "uniform/lightvec.txt";
	ifstream input_file(lightFile);

	double x, y, z;
	int test;
	vector<Point3D> lights;
	while (input_file >> x >> y >> z)
	{
		uniformLight.push_back(Point3D(x, y, z));
	}

}


void loadDenominatorImage(){
	string lightFile = pathName + "denominateur image/light.txt";
	ifstream input_file(lightFile);

	double x, y, z;

	input_file >> x >> y >> z;
	dmLight = Point3D(x, y, z);

	input_file >> x;
	dmPosition = x;

	input_file.close();

	string addon = pathName + "denominateur image/image.bmp";
	char* nameImage = (char*)(addon).c_str();
	if ((dmImage = readBMP(nameImage, width, height)) == NULL) {
		cout << "error  \n" << nameImage;
		return;
	}
}


void createUniformImage(){



	

	list<int> closest[icosahedronSize];
	int lightSize = lights.size();

	//compute closest to uniform light
	for (size_t i = 0; i < lightSize; i++)
	{
		float max = 10;
		int pos = -1;
		Point3D current = lights[i];
		for (size_t j = 0; j < icosahedronSize; j++)
		{
			float dist = current.dist(icosahedron[j]);
			if (dist < max){
				max = dist;
				pos = j;
			}
			
		}
		closest[pos].push_back(i);
	}
		
	
	const int bmpSize = width * height * 3;
	float *falseBitmap = new float[bmpSize];
	unsigned char *trueBitmap = new unsigned char[bmpSize];
	unsigned char *data;
	
	for (size_t i = 0; i < icosahedronSize; i++)
	{
		
		list<int> current = closest[i];
		Point3D currentLight = icosahedron[i];
		
		if (current.size()){
			for (size_t j = 0; j < bmpSize; j++)
			{
				falseBitmap[j] = 0;

			}
			float sum = 0;
			for (list<int>::iterator it = current.begin(); it != current.end(); it++)
			{

				float lightProduct = currentLight.time(lights[*it]);
				string addon = pathName + createFileName(*it+1);
				char* nameImage = (char*)(addon).c_str();
				if ((data = readBMP(nameImage, width, height)) == NULL) {
					cout << "error  \n" << createFileName(*it+1) << " \n" << nameImage;
					return;
				}

				sum += lightProduct;

				for (size_t w = 0; w < bmpSize; w++)
				{
					falseBitmap[w] += lightProduct*(int)data[w];
				}

			}



			for (size_t w = 0; w < bmpSize; w++)
			{
				trueBitmap[w] = (unsigned char)((int)falseBitmap[w] / sum);
			}
			string nameImage = (pathName + "uniform/image" + to_string(uniformSize) + ".bmp");

			writeBMP((char*)(nameImage).c_str(), width, height, trueBitmap);
			uniformSize++;
			uniformLight.push_back(currentLight);
		}
	}

	string filepath = pathName  +"uniform/lightvec.txt";
	ofstream myfile;
	remove(filepath.c_str());
	myfile.open(filepath);
	
	for (vector<Point3D>::iterator it = uniformLight.begin(); it != uniformLight.end(); ++it)
	{
		myfile << it->x << " " << it->y << " " << it->w << "\n";
	}
	myfile.close();
}



void createDenominatorImage(double L , double H) {
	int lightSize = uniformLight.size();
	const int bmpSize = width * height;
	vector<intensityAndimage> *intensityImage = new vector<intensityAndimage>[bmpSize];
	list<int> *forIntensityMean = new list<int>[lightSize];
	vector<intensityAndimage> intensityMean = vector< intensityAndimage>(lightSize);
	vector<intensityAndimage> Kil = vector< intensityAndimage>(lightSize);

	double *trueMean = new double[lightSize];


	for (size_t i = 0; i < lightSize; i++)
	{
		unsigned char* data;
		string addon = pathName+"uniform/image" + to_string(i)+".bmp";
		char* nameImage = (char*)(addon).c_str();
		if ((data = readBMP(nameImage, width, height)) == NULL) {
			cout << "error  \n" << createFileName(i + 1) << " \n" << nameImage;
			return;
		}
		for (size_t j = 0; j < bmpSize; j++)
		{
			int intensity = (int)sqrt((int)data[j * 3] * (int)data[j * 3] + (int)data[j * 3 + 1] * (int)data[j * 3 + 1] + (int)data[j * 3 + 2] * (int)data[j * 3 + 2]);
			intensityImage[j].push_back(intensityAndimage(intensity, i));
			forIntensityMean[i].push_back(intensity);
		}
	}
	
	
	
	for (size_t i = 0; i < lightSize; i++)
	{
		Kil[i] = intensityAndimage(0,i);
	}

	for (size_t i = 0; i < bmpSize; i++)
	{

		std::sort(intensityImage[i].begin(), intensityImage[i].end(), compareiai);
		int intensity = intensityImage[i][lightSize*L].intensity;
		for (size_t j = lightSize*L + 1; j < lightSize; j++)
		{
			if (intensityImage[i][j].intensity>intensity)
				Kil[intensityImage[i][j].image].intensity++;
		}
	}




	for (size_t i = 0; i < lightSize; i++)
	{
		int sum = 0;
		for (list<int>::iterator it = forIntensityMean[i].begin(); it != forIntensityMean[i].end(); it++){
			sum += *it;
		}
		intensityMean[i] = intensityAndimage(sum / bmpSize , i );
	}

	
	std::sort(intensityMean.begin(), intensityMean.end(), compareiai);
	vector<intensityAndimage>::iterator it = intensityMean.begin();

	for (size_t i = 0; i < lightSize; i++)
	{
		trueMean[it->image] = i / (double)lightSize;
		it++;
	}


	
	
	bool flag = true;
	sort(Kil.begin(), Kil.end(), compareiai);
	while (flag){
		intensityAndimage best =  Kil[Kil.size()-1];
		Kil.pop_back();
		int placeImageMean = best.image;
		if (trueMean[placeImageMean]< 0.9){
			dmLight = lights[placeImageMean];

			string addon = pathName +"uniform/image"+ to_string(placeImageMean )+".bmp";
			char* nameImage = (char*)(addon).c_str();
			if ((dmImage = readBMP(nameImage, width, height)) == NULL) {
				cout << "error  \n" << createFileName(placeImageMean + 1) << " \n" << nameImage;
				return;
			}
	
			flag = false;


			//add denominator image at the end to exclude it for the next operations.
			//We need to rewrite the lightvec file so the order of the image and light vectors still are the same

			if (placeImageMean != uniformLight.size()){

				uniformLight[placeImageMean] = uniformLight[uniformLight.size() - 1];
				uniformLight[uniformLight.size() - 1]=dmLight;
				string filepath = pathName + "uniform/lightvec.txt";
				ofstream myfile;
				remove(filepath.c_str());
				myfile.open(filepath);

				for (vector<Point3D>::iterator it = uniformLight.begin(); it != uniformLight.end(); ++it)
				{
					myfile << it->x << " " << it->y << " " << it->w << "\n";
				}
				myfile.close();

				
				unsigned char *lastI;
				string lastAddon = pathName + "uniform/image" + to_string(uniformLight.size() - 1) + ".bmp";
				char* lastNameImage = (char*)(lastAddon).c_str();
				if ((lastI = readBMP(lastNameImage, width, height)) == NULL) {
					cout << "error  \n" << createFileName(placeImageMean + 1) << " \n" << nameImage;
					return;
				}

				writeBMP(lastNameImage, width, height, dmImage);
				writeBMP(nameImage, width, height, lastI);


			}

			

		



			//for test
			string nI = (pathName + "denominateur image/image.bmp");
			writeBMP((char*)(nI).c_str(), width, height, dmImage);

			ofstream myfile;
			nI = (pathName + "denominateur image/light.txt");
			remove(nI.c_str());
			myfile.open(nI);

			myfile << dmLight.x << " " << dmLight.y << " " << dmLight.w << "\n"<<placeImageMean;

		}
	}
	
}


void createNormalEstimation(){
	normalEstimation = new Point3D[width*height];
	int bmpSize = width*height;
	int size = uniformLight.size();
	double ***a = new double**[bmpSize];
	for (size_t j = 0; j < bmpSize; j++)
	{
		a[j] = new double*[size];
		for (size_t i = 0; i < size; i++)
		{
			a[j][i] = new double[3];
		}
	}
	
	
	for (size_t i = 0; i < size; i++)
	{
		unsigned char* data;
		string addon = pathName  +"uniform/image"+to_string(i)+".bmp";
		char* nameImage = (char*)(addon).c_str();
		if ((data = readBMP(nameImage, width, height)) == NULL) {
			cout << "error  \n" << i << " \n" << nameImage;
			return;
		}
		Point3D currentLight = uniformLight[i];
		for (size_t j = 0; j < bmpSize; j++)
		{
			double dmI = Point3D(dmImage[j * 3], dmImage[j * 3 + 1], dmImage[j * 3 + 2]).ll();
			double I1 = Point3D(data[j * 3], data[j * 3 + 1], data[j * 3 + 2]).ll();
			a[j][i][0] = I1*dmImage[j * 3] - dmI*data[j * 3];
			a[j][i][1] = I1*dmImage[j * 3+1] - dmI*data[j * 3+1];
			a[j][i][2] = I1*dmImage[j * 3+2] - dmI*data[j * 3+2];

		}
	}



	double* w = new double[3];
	double ** v = new double*[3];
	for (size_t n = 0; n < 3; n++)
	{
		v[n] = new double[3];
	}
	for (size_t i = 0; i < bmpSize; i++){

		for (size_t n = 0; n < 3; n++)
		{
			w[n] =0;
			for (size_t m = 0; m < 3; m++)
			{
				v[n][m] = 0;
			}
		}

		
		
		svdcmp(a[i], size, 3, w, v);

		int min = w[0], pos = 0;
		for (size_t j = 1; j < 3; j++)
		{
			if (min > w[j]){
				min = w[j];
				pos = j;
			}
		}

		
		
		normalEstimation[i] = Point3D(v[0][pos], v[1][pos], v[2][pos]);
		
		
		
	}
}


void normalRefinement(){

}

void saveNormal(){
	string filepath = "../matlab/normal.txt";
	ofstream myfile;
	remove(filepath.c_str());
	myfile.open(filepath);

	for (int i = 0; i < height*width;i++)
	{
		myfile << normalEstimation[i].x << " " << normalEstimation[i].y << " " << normalEstimation[i].w << "\n";
	}
	myfile.close();
}

int main(int	argc,
	char**	argv)
{
	int choice;
	getWidthHeight();
	loadLight();
	cout << "1 = compute uniform image \n 2 = load uniform image\n";
	cin >> choice;
	if (choice == 1){
		createUniformImage();
	}
	else {
		loadUniformLight();
	}

	cout << "1 = compute denominator Image \n 2 = load denominator Image\n";
	cin >> choice;
	if (choice == 1){
		createDenominatorImage(0.7, 0.9);
	}
	else {
		loadDenominatorImage();
	}
	

	
	lights.clear();

	createNormalEstimation();

	normalRefinement();
	
	saveNormal();

	cout << "\nend bitch\n";
	cin >> argc;
	return 0;
}

//used to create icosahedron form
