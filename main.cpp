//
// Created by zivben on 4/23/17.
//


#include <GeomScore.h>
#include "ChemLib.h"
#include "ChemMolecule.h"
#include "Surface.h"
#include "connolly_surface.h"
#include "MoleculeGrid.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

#define MAX_TRANS_NUM 100

float max_penetration = -2.5;
float penetration_thr = -5.0; //
float ns_thr = 0.6;
vector<int> weights = {-4, -2, 0, 1, 0}; //-4 -2 0 1 0 [1.0-up],[-1.0,1.0], [-2.2,-1.0], [-3.6,-2.2], [-5.0,-3.6]


int getDirs (string dir, vector<string> &files)
{
	DIR *dp;
	struct dirent *dirp;
	
	if((dp = opendir(dir.c_str())) == NULL)
	{
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}
	while ((dirp = readdir(dp)) != NULL)
		files.push_back(dir + string(dirp->d_name));
	closedir(dp);
	return 0;
}

int getTransFiles (string dir, vector<string> &files, string &orgFile)
{
	DIR *dp;
	struct dirent *dirp;
	
	if((dp = opendir(dir.c_str())) == NULL)
	{
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}
	while ((dirp = readdir(dp)) != NULL)
	{
//		std::string prefix("--foo=");
		if ((!string(dirp->d_name).compare(0, 3, "adp")) && (!string(dirp->d_name).compare(string
				                                                                                   (dirp->d_name).length()-3, 3, "pdb")))
			files.push_back(dir + string(dirp->d_name));
		else
			orgFile = (dir + string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

int main(int argc, char *argv[])
{
	char const* CHEMLIB_PATH = "/cs/bio3d/dina/libs/DockingLib/chem.lib";
	
	ChemLib chemLibPath(CHEMLIB_PATH);
	
	vector<ChemMolecule> pdbs;
	vector<Surface> surfaces;
	vector<MoleculeGrid> grids;
	vector<vector<RigidTrans3> > transforms;
	
	string dir = string(argv[1]);
	vector<string> transDirs = vector<string>();
	getDirs(dir, transDirs);
	
	for(unsigned int i=0; i<transDirs.size(); i++)
	{
		
		dir = string(transDirs[i]);
		string orgFile;
		vector<string> transFiles = vector<string>();
		
		getTransFiles(dir, transFiles, orgFile);
		
		std::ifstream is(orgFile, std::ifstream::binary);
		ChemMolecule pdb;
		pdb.loadMolecule(is, chemLibPath);
		pdbs.push_back(pdb);
		cout << pdb.size() << endl;
		is.close();
		Surface pdbSurface = get_connolly_surface(pdb, 10, 1.8);
		MoleculeGrid *grid = new MoleculeGrid(pdbSurface, 0.5, 6);
		surfaces.push_back(pdbSurface);
		grids.push_back(*grid);
		
		
		Molecule<Atom> origMol;
        Molecule<Atom> transMol;

		// calculate transforms
		ifstream origMolFile(orgFile);
		origMol.readPDBfile(origMolFile, PDB::CAlphaSelector());
		Match match;
        for(unsigned int i=0; i<origMol.size();i++)  {
	        match.add(i, i);
        }
		
		
		vector<RigidTrans3> trans;
		for(unsigned int j=0; j < std::min(MAX_TRANS_NUM, (int)transFiles.size()); j++)
		{
			ifstream transMolFile(transFiles[j]);
			transMol.readPDBfile(transMolFile, PDB::CAlphaSelector());
			match.calculateBestFit(origMol, transMol);
			RigidTrans3 t = match.rigidTrans();
			trans.push_back(t);
		}
		transforms.push_back(trans);
	}
	
	// loop over subunits
	for (int i = 0; i <pdbs.size() ; ++i)
	{
		for (int j = i+1; j < pdbs.size() ; ++j)
		{
			// create GeomScore object
			GeomScore geomScore(surfaces[j], &grids[i], weights, penetration_thr, 0.5);
			// loop over transformations for each subunits pair
			for (int t1 = 0; t1 < transforms[i].size(); ++t1)
			{
				for (int t2 = 0; t2 < transforms[j].size(); ++t2)
				{
					// compose the transformation
					RigidTrans3 t = (!transforms[i][t1])*transforms[j][t2];
					bool isClash = geomScore.isPenetrating(t);
					int shapeComplementarityScore = geomScore.score(t);
				}
			}
			
		}
	}
	

}