/*
 * Developing brain Region Annotation With Expectation-Maximization (Draw-EM)
 *
 * Copyright 2013-2016 Imperial College London
 * Copyright 2013-2016 Antonios Makropoulos
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mirtkCommon.h>
#include <mirtkOptions.h>

#include <mirtkImageIOConfig.h>
#include <mirtkGenericImage.h>

#include <string>

using namespace mirtk;
using namespace std;


// =============================================================================
// Help
// =============================================================================

// -----------------------------------------------------------------------------
void PrintHelp(const char *name)
{
	cout << endl;
	cout << "Usage: " << name << " <init_labels> <mask> <N> <probmap_1> <label_1> .. <probmap_N> <label_N> <output> [<init_probmap> <output_probmap_1> .. <output_probmap_N>]" << endl;
	cout << endl;
	cout << "Description:" << endl;
	cout << "  Changes the labels of the <init_labels> image inside the <mask> image" << endl;
	cout << "  according to probability maps <probmap_1> .. <probmap_N> with labels <label_1> .. <label_N> respectively and saves the result in <output>"<<endl;
	cout << endl;
	cout << "Optional: " << endl;
	cout << endl;
	cout << "  Split also the initial probability map <init_probmap> according to the \"membership\" of the probability maps <probmap_1> .. <probmap_N> and add it to the probability maps." << endl;
	cout <<			"  The new probability maps will be saved to  <output_probmap_1> .. <output_probmap_N>" << endl;
	PrintStandardOptions(cout);
	cout << endl;
}

// =============================================================================
// Main
// =============================================================================

// -----------------------------------------------------------------------------


int main(int argc, char **argv)
{
	int x, y, z;

	REQUIRES_POSARGS(6);

	InitializeImageIOLibrary();
	RealImage img;
	BinaryImage mask;

	img.Read(POSARG(1));
	mask.Read(POSARG(2));

	int N=atoi(POSARG(3));
	int labels[N];
	RealImage probs[N];

	int a=4;
	for(int i=0;i<N;i++){
		probs[i].Read(POSARG(a)); a++;
		labels[i] = atoi(POSARG(a)); a++;
	}

	string outputname;
	outputname = POSARG(a); a++;

	bool calcprobs=false;
	RealImage prob;
	string outputprobs[N];
	if(a < NUM_POSARGS){
		calcprobs=true;
		prob.Read(POSARG(a)); a++;

		for(int i=0;i<N;i++){
			outputprobs[i]=POSARG(a); a++;
		}
	}



	for (x = 0; x < img.GetX(); x++) {
		for (y = 0; y < img.GetY(); y++) {
			for (z = 0; z < img.GetZ(); z++) {
				int mval=mask.Get(x,y,z);
				if(mval==0)continue;

				int exlabel=img.Get(x,y,z);
				double maxval=0.0, sum=0.0, newprob=0.0;
				int maxlabel=0;
				double vals[N];

				for(int i=0;i<N;i++){
					vals[i]=probs[i].Get(x,y,z);
					if(vals[i]>maxval){
						maxlabel=labels[i];
						maxval=vals[i];
					}
					sum+=vals[i];
				}
				img.Put(x,y,z,maxlabel);


				if(calcprobs){
					for(int i=0;i<N;i++){
						newprob=vals[i] + vals[i] / sum * prob.Get(x,y,z);
						probs[i].Put(x,y,z, newprob);
					}
				}


			}
		}
	}


	img.Write(outputname.c_str());
	if(calcprobs){
		for(int i=0;i<N;i++) probs[i].Write(outputprobs[i].c_str());
	}

	return 0;
}

