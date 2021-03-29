#include <iostream>
#include <fstream>     
#include "utilities.h" 
#include <sstream>
#include <string>
#include <vector>
#include <time.h>
#include <thread>
#include <algorithm>
#include <stdio.h>

using namespace std;

// écrire 1 si on veut afficher les chemins dans le terminal a evité lorsqu'on veut faire des moyennes
bool affichageresult = 0;

RandGen G;

// déclaration de la structure data 
struct Data{
	static const int nmax=10000; // nombre de noeud max
	static const int tailleTableau = nmax;	// taille des tableaux 	
	static const int coordonneesMax = 100;  // nos points sont de coordonnées comprise entre 0 et 100
	int n=100; // nombre de noeud
	int X[nmax]; // tableau des coordonnées X des noeuds
	int Y[nmax]; // tableau des coordonnées Y des noeuds
	float distance[nmax][nmax]; // tableau des distance
	int coordmin = 0; // minimum des coordonnées
	int coordmax = 100;// maximum des coordonnées
};

Data d;

int creationCoordonnees(){
	// Déclare un générateur aléatoire Gen     

	// Initialise Gen avec un entier quelconque     
	// Nécessaire si on veut les mêmes résultats à chaque exécution     
	        
	// Remplit X et Y avec des nombres réels aléatoires dans [coordmin,coordmax]     
	for (int i = 1; i <= d.n; i++){
		d.X[i]=G.RandInt(d.coordmin,d.coordmax);
		d.Y[i]=G.RandInt(d.coordmin,d.coordmax);
		//cout << d.X[i] << endl;			
	}
	//calcule les distance entre les différent point
	for (int i = 1; i <= d.n; i++){
		for (int j = 1; j <= d.n; j++){
			d.distance[i][j]=sqrt((d.X[i]-d.X[j])*(d.X[i]-d.X[j])+(d.Y[i]-d.Y[j])*(d.Y[i]-d.Y[j]));
			//cout << "distance entre " << i << " et " << j << " est de "<< d.distance[i][j] << endl;
	//cout << d.X[i] << endl;			
		}
	}
	return 0;
}



double PPV(){
	double longeurTotal=0; 
	int imin = 1; // lepoint le pluis pres
	int temp; // variable temporaire de stockage du potentiel imin
	double distanceMin; // plus petite distance entre imin et un autre point dispo
	bool visite[d.nmax]; // 0 si non visité et 1 sinon
	visite[1]=1;
	int chemin[d.nmax]; // tableau de nos chemin
	chemin[1]=1;
	// initialisation de notre tableau de visite a non visite
	for (int i = 2; i <= d.n; i++){
		visite[i]=0;
	}
	// recherche du point le plus près 
	for (int i = 2; i <= d.n; i++){
		distanceMin=1000000;
		for (int j = 2; j <= d.n; j++){
			if (visite[j]==0 && d.distance[imin][j]<distanceMin){
				distanceMin=d.distance[imin][j];
				temp=j; 
			}
		}
		// mise a jours de la longeur
		longeurTotal=longeurTotal+d.distance[imin][temp];
		imin=temp;
		//verouillage le ville
		visite[imin]=1;
		//cout << imin;
		chemin[i]=imin;		
	}
	// ajout du retour au dépot
	chemin[d.n+1]=1;
	//MAJ de la longeur total
	longeurTotal=longeurTotal+d.distance[chemin[d.n]][chemin[d.n+1]];
	// affichage
	if (affichageresult == 1){
		cout << endl;
		cout << "Le chemin du probleme avec PPV est : " ;
		for (int i=1; i<=d.n; i=i+1)  cout << chemin[i] << " -> ";
		cout << chemin[d.n+1] << endl;
		cout << "La longeur Total est de : " << longeurTotal;	
	}
	/*
	//Création du CSV
	// open a file in write mode.
	ofstream outfile;
	cout << endl;
	outfile.open("dataPPV.csv");
	//cout << d.X[1] << endl;
	//cout << "Writing data into file \"dataPPV.csv\"..." << endl;
	for (int i = 1; i <= d.n+1; i++) {
		//cout << d.X[i] << endl;	
		outfile << d.X[chemin[i]] << ", " << d.Y[chemin[i]] << endl; //write into the file
 	 }
	outfile.close();*/

	return 	longeurTotal;	
	
}

double PPI(){
	double longeurTotal=0;
	int position;
	int imin = 1; // la ville le plus pres
	int jmin = 1; // la position le plus pres
	double distanceMin; // plus petite distance entre imin et un autre point dispo
	bool visite[d.nmax]; // 0 si non visité et 1 sinon
	vector<int> chemin;
	chemin.push_back(-1); // permet de remplir le chemin[0] pour compté a partir de la case 1
	chemin.push_back(1); // ajout du depot au depart
	// initialisation de notre tableau de visite a non visite sauf pour le depot
	visite[1]=1;
	for (int i = 2; i <= d.n; i++){
		visite[i]=0;
	}
	for (int i = 2; i <= d.n; i++){
		// recherche de la ville la plus proche de notre chemin a inserer
		distanceMin=INT_MAX;
		for (int k = 1; k < chemin.size(); k++){
			for (int j = 2; j <= d.n; j++){
				if (visite[j]==0 && d.distance[chemin[k]][j]<distanceMin){
					distanceMin=d.distance[chemin[k]][j];
					jmin=j; 
				}
			}
		}
		distanceMin=INT_MAX;
		if (chemin.size()==2){// cas de première insertion si aucune ville autre que la 1 n'a été inseré on na pas le choix de la position
			chemin.push_back(jmin);
			longeurTotal = longeurTotal + d.distance[chemin[1]][jmin] + d.distance[jmin][1];
		} else {
			// recherche de la meilleur position pour la ville choisi
			for (int k = 1; k <= chemin.size()-1; k++){
				if (k <= chemin.size()-2){ // Si on est pas au bout de la chaine déja créer
				// calcul du nouveau trajet 
					if (d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[k+1]]-d.distance[chemin[k]][chemin[k+1]] < distanceMin){
						//exemple ajout de 3 entre 1 et 2
						distanceMin=d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[k+1]]-d.distance[chemin[k]][chemin[k+1]];
						//  d =        1->3                    +          3  -> 2            -      1   ->   2
						position=k; 
					}		
				}
				else{//insertion avant le retour au depot
					if (d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[1]]-d.distance[chemin[k]][chemin[1]] < distanceMin){
						distanceMin=d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[1]]-d.distance[chemin[k]][chemin[1]];
						position=k; 
					}						
				}
			}
			// MAJ longeur total et insertion
			longeurTotal=longeurTotal+distanceMin;
			chemin.insert(chemin.begin()+position+1,jmin);		
		}
		//verouillage de la ville
		visite[jmin]=1;
	}	
	// ajout du depot
	chemin.push_back(1);
	// affichage
	if (affichageresult == 1){
		cout << endl;
		cout << "Le chemin du probleme avec PPI est : " ;
		for (int i=1; i<=d.n; i=i+1)  cout << chemin[i] << " -> ";
		cout << chemin[d.n+1] << endl;
		cout << "La longeur Total est de : " << longeurTotal;	
	}
	
	
	/*
	//Création du CSV
	// open a file in write mode.
	ofstream outfile;
	cout << endl;
	outfile.open("dataPPI.csv");
	//cout << d.X[1] << endl;
	//cout << "Writing data into file \"dataPPI.csv\"..." << endl;
	for (int i = 1; i <= d.n+1; i++) {
		//cout << d.X[i] << endl;	
		outfile << d.X[chemin[i]] << ", " << d.Y[chemin[i]] << endl; //write into the file
 	 }
	outfile.close();*/

	return 	longeurTotal;	
	
}

double PLI(){
	double longeurTotal=0;
	int position;
	int imin = 1; // la ville le plus pres
	int jmin = 1; // la position le plus pres
	double distanceMin; // plus petite distance entre imin et un autre point dispo
	double distanceMax; // plus grande distance entre un point et les points de notre liste des ville deja inserer
	bool visite[d.nmax]; // 0 si non visité et 1 sinon
	vector<int> chemin;
	chemin.push_back(-1); // permet de remplir le chemin[0] pour compté a partir de la case 1
	chemin.push_back(1); // ajout du depot
	// initialisation de notre tableau de visite a non visite sauf pour le depot
	visite[1]=1;
	for (int i = 2; i <= d.n; i++){
		visite[i]=0;
	}
	
	for (int i = 2; i <= d.n; i++){
		// recherche de la ville la plus loin de notre chemin a inserer
		distanceMax=0;
		for (int k = 1; k < chemin.size(); k++){
			for (int j = 2; j <= d.n; j++){
				if (visite[j]==0 && d.distance[chemin[k]][j]>distanceMax){
					distanceMax=d.distance[chemin[k]][j];
					jmin=j; 
				}
			}
		}
		distanceMin=INT_MAX;
		if (chemin.size()==2){// cas de première insertion si aucune ville autre que la 1 n'a été inseré on na pas le choix de la position
			chemin.push_back(jmin);
			longeurTotal = longeurTotal + d.distance[chemin[1]][jmin] + d.distance[jmin][1];
		} else {
			// recherche de la meilleur position pour la ville choisi
			for (int k = 1; k <= chemin.size()-1; k++){
				if (k <= chemin.size()-2){ // Si on est pas au bout de la chaine déja créer
				// calcul du nouveau trajet 
					if (d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[k+1]]-d.distance[chemin[k]][chemin[k+1]] < distanceMin){
						//exemple ajout de 3 entre 1 et 2
						distanceMin=d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[k+1]]-d.distance[chemin[k]][chemin[k+1]];
						//  d =             1->3              +          3  -> 2         -      1   ->   2
						position=k; 
					}		
				}
				else{//insertion avant le retour au depot
					if (d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[1]]-d.distance[chemin[k]][chemin[1]] < distanceMin){
						distanceMin=d.distance[chemin[k]][jmin]+d.distance[jmin][chemin[1]]-d.distance[chemin[k]][chemin[1]];
						position=k; 
					}						
				}
			}
			// MAJ longeur total et insertion
			longeurTotal=longeurTotal+distanceMin;
			chemin.insert(chemin.begin()+position+1,jmin);		
		}
		//cout << "longeur total " << longeurTotal << endl;
		visite[jmin]=1;
	}	
	// ajout du depot
	chemin.push_back(1);
	// affichage
	if (affichageresult == 1){
		cout << endl;
		cout << "Le chemin du probleme avec PLI est : " ;
		for (int i=1; i<=d.n; i=i+1)  cout << chemin[i] << " -> ";
		cout << chemin[d.n+1] << endl;
		cout << "La longeur Total est de : " << longeurTotal;	
	}
	
	
	/*
	//Création du CSV
	// open a file in write mode.
	ofstream outfile;
	cout << endl;
	outfile.open("dataPLI.csv");
	//cout << d.X[1] << endl;
	//cout << "Writing data into file \"dataPLI.csv\"..." << endl;
	for (int i = 1; i <= d.n+1; i++) {
		//cout << d.X[i] << endl;	
		outfile << d.X[chemin[i]] << ", " << d.Y[chemin[i]] << endl; //write into the file
 	 }
	outfile.close();*/

	return 	longeurTotal;	
	
}




double MI(){
	double longeurTotal=0;
	double variationLongeurTemp=0;
	int position;
	int imin = 1; // la ville le plus pres
	int jmin = 1; // la position le plus pres
	double distanceMin; // plus petite distance entre imin et un autre point dispo
	bool visite[d.nmax]; // 0 si non visité et 1 sinon
	vector<int> chemin;
	chemin.push_back(-1); // permet de remplir le chemin[0] pour compté a partir de la case 1
	chemin.push_back(1);// ajout du depot
	//cout << "debut de mI" << endl;
	// initialisation de notre tableau de visite a non visite sauf pour le depot
	visite[1]=1;
	for (int i = 2; i <= d.n; i++){
		visite[i]=0;
	}
	// recherche parmis toute les villes celle qui améliore le plus de notre chemin
	for (int i = 2; i <= d.n; i++){
		
		variationLongeurTemp=INT_MAX;
		if (chemin.size()==2){// cas de première insertion si aucune ville autre que la 1 n'a été inseré on na pas le choix de la position
			for (int j=2;j <= d.n; j++){
				if (d.distance[1][j] + d.distance[j][1]<variationLongeurTemp){
					variationLongeurTemp=d.distance[1][j] + d.distance[j][1];
					position=2;
					jmin=j;
				
				}
			}
		   // insertion dans la seul position possible de la ville choisi pour le premier cas il sagit juste de trouver la vile qui entre 1 et 1 coute le moin donc c'est la ville la plus proche de 1
			visite[jmin]=1;
			longeurTotal=longeurTotal+variationLongeurTemp;
			chemin.insert(chemin.begin()+position,jmin);
		}else {
			// recherche parmis toute les villes celle qui améliore le plus de notre chemin
			for (int j=2;j <= d.n; j++){ // pour toute les villes
				//si la ville n'est pas visité
				if (visite[j]==0){
					// on test sa meilleur insertion
					for (int k = 1; k <= chemin.size()-1; k++){
						// en fonction de sa position d'insertion le calcul change mais on garde celle qui minimise la variation de longeur et donc la longeur total
						if (k <= chemin.size()-2){
							if (d.distance[chemin[k]][j]+d.distance[j][chemin[k+1]]-d.distance[chemin[k]][chemin[k+1]]<=variationLongeurTemp){
								variationLongeurTemp=d.distance[chemin[k]][j]+d.distance[j][chemin[k+1]]-d.distance[chemin[k]][chemin[k+1]];
								position=k;
								jmin=j;
							}
						}else{
							if (d.distance[chemin[k]][j]+d.distance[j][chemin[1]]-d.distance[chemin[k]][chemin[1]] <= variationLongeurTemp){
								variationLongeurTemp=d.distance[chemin[k]][j]+d.distance[j][chemin[1]]-d.distance[chemin[k]][chemin[1]];
								position=k;
								jmin=j;
							}
						}		
					}
					 
				}
			}
			// on sauvegarde la ville en sa position qui minimise la longeur total et on la verouille et on met a jour la longeur total
			visite[jmin]=1;
			longeurTotal=longeurTotal+variationLongeurTemp;
			chemin.insert(chemin.begin()+position+1,jmin);				
		}
	}	
	// on ajoute le depot
	chemin.push_back(1);
	//affichage
	if (affichageresult == 1){
		cout << endl;
		cout << "Le chemin du probleme avec MI est : " ;
		for (int i=1; i<=d.n; i=i+1)  cout << chemin[i] << " -> ";
		cout << chemin[d.n+1] << endl;
		cout << "La longeur Total est de : " << longeurTotal;
	}
	
	/*
	//Création du CSV
	// open a file in write mode.
	ofstream fichier("dataMI.csv",ios::out | ios::trunc);
	cout << endl;
	//cout << d.X[1] << endl;
	//cout << "Writing data into file \"dataMI.csv\"..." << endl;
	for (int i = 1; i <= d.n+1; i++) {
		//cout << d.X[i] << endl;	
		fichier << d.X[chemin[i]] << ", " << d.Y[chemin[i]] << endl; //write into the file
 	}
	fichier.close();*/

	return 	longeurTotal;
	
}


int main(int argc, char** argv) {
	G.Init(875515137);  // aléatoire
	clock_t time1,time2; // timer
	d.n=50; // nombre de noeud	
	int m=100; // nombre de tours pour les stats
	d.coordmin = 0;
	d.coordmax = 100;
	affichageresult=0; // écrire 1 si on veut afficher les chemins dans le terminal a evité lorsqu'on veut faire des moyennes
	
	//tableau des resultats en distance des différents algo
	double ResultPPV[m]; 
	double ResultPPI[m];
	double ResultPLI[m];
	double ResultMI[m];
	//tableau des temps en ms des différents algo	
	double tempsPPV[m];
	double tempsPPI[m];
	double tempsPLI[m];
	double tempsMI[m];
	// moyenne des resultats en distance des différents algo
	double moyPPV=0;
	double moyPPI=0;
	double moyPLI=0;
	double moyMI=0;
	// moyenne des temps en ms des différents algo	
	double moytPPV=0;
	double moytPPI=0;
	double moytPLI=0;
	double moytMI=0;
	//tableau des classements des resultats des différents algo
	int premier[5]={0,0,0,0,0};
	int deuxieme[5]={0,0,0,0,0};
	int troisieme[5]={0,0,0,0,0};
	int quatrieme[5]={0,0,0,0,0};	
	
	// boucle pour les stats
	for (int k=1; k<=m; k++){
		// creation des données aléatoire
		creationCoordonnees();
        cout << "Tours "<<k << endl;
		//exemple1()
		//exportCSV();
		//plotData();
		
		//lancement PPV
		time1 = clock();
		ResultPPV[k]=PPV();
		time2 = clock();
		tempsPPV[m]=(float)(time2-time1);
		//plotDataPPV();
		
		//lancement PPI
		time1 = clock();
		ResultPPI[k]=PPI();
		time2 = clock();
		tempsPPI[m]=(float)(time2-time1);
		//plotDataPPI();
		
		//lancement PLI
		time1 = clock();
		ResultPLI[k]=PLI();
		time2 = clock();
		tempsPLI[m]=(float)(time2-time1);
		//plotDataPLI();
		
		//lancement MI
		time1 = clock();
		ResultMI[k]=MI();
		time2 = clock();
		tempsMI[m]=(float)(time2-time1);
		//plotDataMI();


		// recherche du classements des resultats
		// recherche du meilleur
		double result[5]={1000000,ResultPPV[k],ResultPPI[k],ResultPLI[k],ResultMI[k]};
		int q = sizeof(result) / sizeof(result[0]);
		double minimum= *min_element(result, result + q);
		if(minimum == ResultPPV[k]){
	        result[0]=0;
	        result[1]=0;
	        premier[1]=premier[1]+1;
	    }
	    if(minimum == ResultPPI[k]){
	        result[0]=0;
	        result[2]=0;
	        premier[2]=premier[2]+1;
	    }
	    if(minimum == ResultPLI[k]){
	        result[0]=0;
	        result[3]=0;
	        premier[3]=premier[3]+1;
	    }
	    if(minimum == ResultMI[k]){
	        result[0]=0;
	        result[4]=0;
	        premier[4]=premier[4]+1;
	    }
	    // recherche du pire
		q = sizeof(result) / sizeof(result[0]);
		double maximum= *max_element(result, result + q);
		if(maximum == ResultPPV[k]){
	        quatrieme[1]=quatrieme[1]+1;
	        result[1]=0;
	    }
	    if(maximum == ResultPPI[k]){
	        quatrieme[2]=quatrieme[2]+1;
	        result[2]=0;
	    }
	    if(maximum == ResultPLI[k]){
	        quatrieme[3]=quatrieme[3]+1;
	        result[3]=0;
	    }
	    if(maximum == ResultMI[k]){
	        quatrieme[4]=quatrieme[4]+1;
	        result[4]=0;
	    }
	    
	    // recherche du troisième meilleur
		q = sizeof(result) / sizeof(result[0]);
		maximum= *max_element(result, result + q);
		if(maximum == ResultPPV[k]){
	        troisieme[1]=troisieme[1]+1;
	        result[1]=0;
	    }
	    if(maximum == ResultPPI[k]){
	        troisieme[2]=troisieme[2]+1;
	        result[2]=0;
	    }
	    if(maximum == ResultPLI[k]){
	        troisieme[3]=troisieme[3]+1;
	        result[3]=0;
	    }
	    if(maximum == ResultMI[k]){
	        troisieme[4]=troisieme[4]+1;
	        result[4]=0;
	    }	    
	    // recherche du deuxième meilleur
		q = sizeof(result) / sizeof(result[0]);
		maximum= *max_element(result, result + q);
		if(maximum == ResultPPV[k]){
	        deuxieme[1]=deuxieme[1]+1;
	        result[1]=0;
	    }
	    if(maximum == ResultPPI[k]){
	        deuxieme[2]=deuxieme[2]+1;
	        result[2]=0;
	    }
	    if(maximum == ResultPLI[k]){
	        deuxieme[3]=deuxieme[3]+1;
	        result[3]=0;
	    }
	    if(maximum == ResultMI[k]){
	        deuxieme[4]=deuxieme[4]+1;
	        result[4]=0;
	    }	 
	}
	
	//affichage
	
	cout << endl;
	for (int i = 1; i <= m; i++){
		cout << "PPV : " <<ResultPPV[i] << " | PPI : " <<ResultPPI[i] << " | PLI : " << ResultPLI[i] << " | MI : " <<ResultMI[i]<< endl;
	}
	for (int i = 1; i <= m; i++){
		moyPPV = moyPPV + ResultPPV[i]/m;
		moyPPI = moyPPI + ResultPPI[i]/m;
		moyPLI = moyPLI + ResultPLI[i]/m;
		moyMI = moyMI + ResultMI[i]/m;
	}
	
	for (int i = 1; i <= m; i++){
		moytPPV = moytPPV + tempsPPV[i]/m;
		moytPPI = moytPPI + tempsPPI[i]/m;
		moytPLI = moytPLI + tempsPLI[i]/m;
		moytMI = moytMI + tempsMI[i]/m;
	}	

	cout<< endl;
	cout << "les resultats moyens en fonction des techniques sont :";
	cout << endl;
	cout << "nom |\t moy \t|\t1er\t|\t2eme\t|\t3eme\t|\t4eme\t|\t temps_exec" << endl;
	cout << "PPV |\t" <<moyPPV << "\t|\t" << premier[1] << "\t|\t" << deuxieme[1] <<"\t|\t" << troisieme[1] <<"\t|\t" << quatrieme[1] << "\t|\t"<<moytPPV << " millisec " << endl;
	cout << "PPI |\t" <<moyPPI << "\t|\t" << premier[2] << "\t|\t" << deuxieme[2] <<"\t|\t" << troisieme[2] <<"\t|\t" << quatrieme[2] << "\t|\t"<<moytPPI<<" millisec " <<endl;
	cout << "PLI |\t" <<moyPLI << "\t|\t" << premier[3] << "\t|\t" << deuxieme[3] <<"\t|\t" << troisieme[3] <<"\t|\t" << quatrieme[3] << "\t|\t" <<moytPLI<< " millisec " <<endl;
	cout << "MI  |\t" <<moyMI <<  "\t|\t" << premier[4] << "\t|\t" << deuxieme[4] <<"\t|\t" << troisieme[4] <<"\t|\t" << quatrieme[4] << "\t|\t"<<moytMI<< " millisec " <<endl;
	return 0;
}





