#include "/home/t2k/geant4/WCSim_solidworksfinal/include/WCSimRootGeom.hh"
#include "/home/t2k/geant4/WCSim_solidworksfinal/include/WCSimRootEvent.hh"
#include <TObject.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TGraph2D.h>
#include <TEllipse.h>
#include <TLine.h>
#include <stdio.h>     
#include <stdlib.h>
#include <Riostream.h>
#include <iostream>
#include "TClonesArray.h"
#include <math.h>
#include "TApplication.h"
#include <TMath.h>

using namespace std;
using namespace ROOT;
//using namespace Math;

void timingrefC(int argc, char** argv) {

  // gROOT->Reset();
  // char* wcsimdirenv;
  // wcsimdirenv = getenv ("WCSIMDIR");
  // if(wcsimdirenv !=  NULL)
  //   {
  //     gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  //   }
  // else
  //   {
  //     gSystem->Load("../../libWCSimRoot.so");
  //   }
  // gStyle->SetOptStat(1);

  // //	char* wcsimdirenv;
  // 	gStyle->SetOptStat(1);

	TFile *f;
	f = new TFile("/home/t2k/geant4/WCSim_solidworksfinal_build/10_solidworks.root");

	TChain  *wcsimT = new TChain("wcsimT");

	wcsimT->Add("/home/t2k/geant4/WCSim_solidworksfinal_build/10_solidworks.root");
       
	WCSimRootEvent *wcsimrootevent = new WCSimRootEvent();
	wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootevent);
	wcsimT -> GetBranch("wcsimrootevent") -> SetAutoDelete(kTRUE);

	int entries =  wcsimT -> GetEntries();
	TTree *wcsimGeoT = (TTree*) f->Get("wcsimGeoT");
	int nevent = wcsimGeoT->GetEntries();

	WCSimRootGeom *wcsimrootgeom = new WCSimRootGeom();
	TBranch *branch = wcsimGeoT->GetBranch("wcsimrootgeom");
	branch->SetAddress(&wcsimrootgeom);

	wcsimGeoT->SetBranchAddress("wcsimrootgeom", &wcsimrootgeom);

	wcsimGeoT->GetEntry(0);
	int numpmt = wcsimrootgeom->GetWCNumPMT();
	cout << numpmt << endl;

		TCanvas *c1 = new TCanvas("c1", "c1",1000,1000);
       	c1 -> Divide(2,2);

      	TH2D* hist1 = new TH2D("True","Total Reflector hit", 1000, 0 ,329 , 1000, -329.0, +329.0);
	//TH2D* hist2 = new TH2D("r^2phi","R^2 phi projection of start position of photons", 1000, 0 , 360 , 1000, 0, 40);
	//	TH2D* hist3 = new TH2D("X vs Z"," Endpoint of the photons ", 1000,-38 , -24 , 1000, -38, -24);
       	TH2D* hist2 = new TH2D("reflectorhit","same reflectorid & tubeid ", 1000, 0 ,329 , 1000, -329.0, +329.0);
       	TH2D* hist3 = new TH2D("reflectorhit","different reflectorid and tubeid", 1000, 0, 329.0 , 1000, -329, +329);
	TH2D* hist4 = new TH2D("relfectorhit","first reflectorhit for plot3", 1000, 0 , 329.0 , 1000, -329.0, 329.0);
	
	
	for (int k=0; k<entries; k++)
	  {
	    wcsimT -> GetEntry(k);
	     WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
	    int ref = wcsimroottrigger->GetNreflectorhits();
	    int ReflectorID[ref];
	    int ReflectortrackID[ref];
	    double Reflectorx[ref];
	    double Reflectory[ref];
	    double Reflectorz[ref];
	    double Reflectortheta[ref];
	    for (int i=0; i<ref; i++)
	      {  
		WCSimRootReflectorHit *rhit = (WCSimRootReflectorHit*)
		  (wcsimroottrigger->GetReflectorHits()->At(i));
		int reflectorid = rhit -> GetReflectorID();
		int reflectortrackid = rhit -> GetRefTrackID();
		double x_ref = rhit->GetReflectorPos(0)*0.1;
		double y_ref = rhit->GetReflectorPos(1)*0.1;
		double z_ref = rhit->GetReflectorPos(2)*0.1;
		double theta_r =atan(z_ref/x_ref)*180/3.14159;
		double thetaa_r = 0;
		if (z_ref<0 && x_ref>0) {
		  thetaa_r = theta_r+360;
		}
		else if (x_ref<0 && z_ref>0){
		  thetaa_r = theta_r+180;
		}
		else if (x_ref<0 && z_ref<0){
		  thetaa_r = theta_r + 180;
		}
		else if (x_ref>0 && z_ref>0){
		  thetaa_r = theta_r;
		}

		ReflectorID[i] = reflectorid;
		ReflectortrackID[i] = reflectortrackid;
		Reflectorx[i] = x_ref;
		Reflectory[i] = y_ref;
		Reflectorz[i] = z_ref;
		Reflectortheta[i] = thetaa_r;
		
	      
		
	    // WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
	    int max=wcsimroottrigger->GetNcherenkovhits();
	    for (int i = 0; i<max; i++)
	      {
		WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)
		  (wcsimroottrigger->GetCherenkovHits()->At(i)); 
		
		float pe_0 = chit -> GetTotalPe(0);
		float pe_1 = chit -> GetTotalPe(1);
		int  tubeid = chit -> GetTubeID();
		int  trackid = chit -> GetTrackID();
		int iHR = chit -> GetIsHitReflector();
		WCSimRootPMT pmt = wcsimrootgeom ->GetPMT(tubeid-1);
		double X = pmt.GetPosition(0);
		double Y = pmt.GetPosition(1);
		double Z = pmt.GetPosition(2);
		double thetaa = 0;
		double theta =atan(Z/X)*180/3.14159;
		if (Z<0 && X>0) {
		  thetaa = theta+360;
		}
		else if (X<0 && Z>0){
		  thetaa = theta+180;
		}
		else if (X<0 && Z<0){
		  thetaa = theta + 180;
		}
		else if (X>0 && Z>0){
		  thetaa = theta;
		}
		    
		if (Y >= -280.0 && Y <= 280.0) //barrel
		  //  if (Y >= 280.0) //topcap
		  //if (Y <= -280.0) //bottom
		  {
		    if (ReflectortrackID[i] == trackid)
		      {
			double charge =chit -> GetTotalPe(1); 
			hist1->Fill(thetaa, Y, charge);
		      }  
			
		  }
		    if (Y >= -280.0 && Y <= 280.0) //barrel
		    // if (Y >= 280.0) //top
		      //if (Y <= -280.0) //bottom
		      {
			if (ReflectortrackID[i] == trackid)
			  {
			    if (ReflectorID[i] == tubeid)
			      {
				double charge =chit -> GetTotalPe(1); 
				hist2->Fill(thetaa, Y, charge);
			       
			      }
			  }
		      }
		    int temp, temp1;
		    
		    if (Y >= -280.0 && Y <= 280.0) //barrel
		    // if (Y >= 280.0) //top
		      //if (Y <= -280.0) //bottom
		      {
			if (reflectortrackid == trackid)
			  {
			    if (reflectorid != tubeid)
			      {
				temp = trackid;
				
				double charge =chit -> GetTotalPe(1); 
				hist3->Fill(thetaa, Y, charge);
			      }
			    if(ref==0)
			      {
				temp1=temp;
				    
				double charge1 =chit -> GetTotalPe(1); 
				hist4->Fill(thetaa, Y, charge1);
			      }
			    if(temp1!=temp)
			      {
				temp1=temp;
				    
				double charge1 =chit -> GetTotalPe(1); 
				hist4->Fill(thetaa, Y, charge1);
				    //	cout<< ""<<arrayrefid[j]<<" "<<tubeid<<endl;
			      }
			  }
		      }
	      }
		    // if (Y >= -280.0 && Y <= 280.0)// barrel
		    // int temp, temp1;
		    // // if (Y >= 280.0)  //topcap
		    //   //if (Y <= -280.0) //bottom
		    //   {
		    // 	if (reflectortrackid == trackid)
		    // 	  {
			    
		    // 	    if (reflectorid != tubeid)
		    // 	      {
		    // 		temp=trackid;
				
		    // 		if(j==0)
		    // 		  {
		    // 		    temp1=temp;
		    // 		    //cout<<"reflectorid =  "<<reflectorid<<"tubeid =  "<<tubeid<<"trackid = "<<trackid<<endl;
		    // 		    // double charge =chit -> GetTotalPe(1); 
		    // 		   hist4->Fill(thetaa_r, y_ref);
		    // 		  } 
		    // 		if(temp1!=temp)
		    // 		  {
		    // 		    temp1=temp;
		    // 		    // cout<<"reflectorid =  "<<reflectorid<<"tubeid =  "<<tubeid<<"trackid = "<<trackid<<endl;
		    // 		    double charge =chit -> GetTotalPe(1); 
		    // 		    hist4->Fill(thetaa_r, y_ref);
		    // 		    //	cout<< ""<<arrayrefid[j]<<" "<<tubeid<<endl;
		    // 		  }
		    // 	      }
		    // 	  }
		    //   }
		 
		
		 
	      
	      }
	  }
	
	
	
	
	
	cout << "plotted cd 1" << endl;
	c1 -> cd(1);
	hist1-> SetXTitle("Theta");
	hist1->SetYTitle("Y coordinate in cm");
	hist1->Draw("colz");
		cout << "plotted cd 2" << endl;
	c1 -> cd(2);
	hist2-> SetXTitle("theta");
	hist2->SetYTitle("Y coordinate in cm");
	hist2->SetTitleOffset(1.5);
	hist2->Draw("colz");
	c1 -> cd(3);
	hist3-> SetXTitle("theta");
	hist3->SetYTitle("Y coordinate in cm");
	hist3->SetTitleOffset(1.5);
	hist3->Draw("colz");
        c1 -> cd(4);
	hist4-> SetXTitle("theta");
	hist4->SetYTitle("Y coordinate in cm");
	hist4->SetTitleOffset(1.5);
	hist4->Draw("colz");

	return;
}
int main(int argc, char** argv) {
  TApplication app("ROOT Application", &argc, argv);
  timingrefC(app.Argc(), app.Argv());
  app.Run();
  
  // app.Terminate(); // still doesn't quit
  
  return 0;
}
