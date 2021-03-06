#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"
#include "TApplication.h"
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
#include <TMath.h>
#include "Math/Vector3D.h"
//#include <Math/GenVector/Dot.h>
//#include <Math/GenVector/RotationY.h>
//#include <Math/GenVector/RotationZ.h>
#include "TGraph.h"
#include <math.h>
using namespace std;
using namespace ROOT::Math;

void reflectorhit(int argc, char** argv) {

	TFile *f;
	f = new TFile("/home/t2k/geant4/WCSim_geometry1final_build/1000_geometry1.root");

	TChain  *wcsimT = new TChain("wcsimT");

	wcsimT->Add("/home/t2k/geant4/WCSim_geometry1final_build/1000_geometry1.root");
       
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

      	TH1D* hist1 = new TH1D("CherenkovHits_barrelpart","Total Cherenkov hit angle", 1000, -10, 190);
       	TH1D* hist2 = new TH1D("reflectorhit","same reflectorid & tubeid angle ", 1000, -10 ,190);
       	TH1D* hist3 = new TH1D("reflectorhit","different reflectorid and tubeid angle", 1000, -10 , 190);
TH1D* hist4 = new TH1D("reflectorhit","No reflector Hit", 1000, -10 , 190);	
	//	TH2D* hist4 = new TH2D("relfectorhit","first reflectorhit for plot3", 1000, 0 , 329.0 , 1000, -329.0, 329.0);
	
	//	wcsimT->GetEntry(0);
	for (int k=0; k<entries; k++)
	  {
	    wcsimT -> GetEntry(k);
	    WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
	    int max=wcsimroottrigger->GetNcherenkovhits();
	    int ref = wcsimroottrigger->GetNreflectorhits(); 
	    XYZVector pdvector[max];
	    XYZVector vxector[max];
	    XYZVector pr1dvector[ref];
	    XYZVector vr1xector[ref];
	    XYZVector pr2dvector[ref];
	    XYZVector vr2xector[ref];
	    XYZVector pr3dvector[max];
	    XYZVector vr3xector[max];
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
		
		WCSimRootCherenkovHitTime *hittime = (WCSimRootCherenkovHitTime*)
		  (wcsimroottrigger->GetCherenkovHitTimes()->At(i));
		
		  float time = hittime->GetTruetime();
		  float directionx = hittime->GetPhotonDir(0);
		  float directiony = hittime->GetPhotonDir(1);
		   float directionz = hittime->GetPhotonDir(2);
		   double x_start = hittime->GetPhotonStartPos(0)*0.1;
		   double y_start = hittime->GetPhotonStartPos(1)*0.1;
		   double z_start = hittime->GetPhotonStartPos(2)*0.1;
		   double x_end = hittime->GetPhotonEndPos(0)*0.1;
		   double y_end = hittime->GetPhotonEndPos(1)*0.1;
		   double z_end = hittime->GetPhotonEndPos(2)*0.1;
		   double VX = x_end - x_start;
		   double VY = y_end - y_start;
		   double VZ = z_end - z_start;
		   //float length1 = sqrt( directionx* directionx + directiony*directiony +directionz*directionz );
		   float length2 = sqrt(VX*VX + VY*VY + VZ*VZ);
		   double VX1 = VX/length2;
		   double VY1 = VY/length2;
		   double VZ1 = VZ/length2;
		   // if (Y >= -280.0 && Y <= 280.0) //barrel
		   //  {
		       pdvector[i].SetCoordinates(directionx, directiony, directionz);
		       vxector[i].SetCoordinates(VX1, VY1, VZ1);
		       double s = acos((pdvector[i].Dot(vxector[i])));
		       float aa = s*180/3.14;
		       hist1->Fill(aa); 
		       // }
		   if (iHR == 0)
		   {
		     pr3dvector[i].SetCoordinates(directionx, directiony, directionz);
		    vr3xector[i].SetCoordinates(VX1, VY1, VZ1);
		     double s3 = acos((pr3dvector[i].Dot(vr3xector[i])));
		     float aa3 = s3*180/3.14;
		     hist4->Fill(aa3);
		  // 		//cout<<" "<<VX1<<" "<<VY1<<" "<<VZ1<<" "<<reflectorid<< " "<<tubeid<<endl;
		  // 		//cout <<" "<<reflectorid<<" "<<tubeid<<endl;	
	   	   	      } 
		   for (int j=0; j<ref; j++)
		     {
		   
		       WCSimRootReflectorHit *rhit = (WCSimRootReflectorHit*)
			 (wcsimroottrigger->GetReflectorHits()->At(j));
		       int reflectorid = rhit -> GetReflectorID();
		       int reflectortrackid = rhit -> GetRefTrackID();
		       double x_ref = rhit->GetReflectorPos(0)*0.1;
		       double y_ref = rhit->GetReflectorPos(1)*0.1;
		       double z_ref = rhit->GetReflectorPos(2)*0.1;
		       int temp, temp1; 
		       // if (Y >= -280.0 && Y <= 280.0) //barrel
			    // {
	 	if (reflectortrackid == trackid)
	  		  {
	  		    if (reflectorid == tubeid)
	  		      {
				temp = trackid;
				
			      
	  		    if(j==0)
	  		      {
	  			temp1=temp;
				pr1dvector[j].SetCoordinates(directionx, directiony, directionz);
				vr1xector[j].SetCoordinates(VX1, VY1, VZ1);
				double s1 = acos((pr1dvector[j].Dot(vr1xector[j])));
				float aa1 = s1*180/3.14;
				if (aa1 <= 40)
				  {
				    	cout<<" "<<VX1<<" "<<VY1<<" "<<VZ1<<" "<<reflectorid<< " "<<tubeid<<endl;
				  }
				hist2->Fill(aa1);
	  		      }
	 		    if(temp1!=temp)
	  		      {
	 			temp1=temp;
				pr1dvector[j].SetCoordinates(directionx, directiony, directionz);
				vr1xector[j].SetCoordinates(VX1, VY1, VZ1);
				double s1 = acos((pr1dvector[j].Dot(vr1xector[j])));
				float aa1 = s1*180/3.14;
				hist2->Fill(aa1);	
	   		      }
			      }  
		}   
		     int tem, tem1;
		     // if (Y >= -280.0 && Y <= 280.0) //barrel
			  // {
	 	if (reflectortrackid == trackid)
	  		  {
	  		    if (reflectorid != tubeid)
	  		      {
				tem = trackid;
				//	cout <<" "<<reflectorid<<" "<<tubeid<<endl;	
			      
	  		    if(j==0)
	  		      {
	  			tem1=tem;
				pr2dvector[j].SetCoordinates(directionx, directiony, directionz);
				vr2xector[j].SetCoordinates(VX1, VY1, VZ1);
				double s2 = acos((pr2dvector[j].Dot(vr2xector[j])));
				float aa2 = s2*180/3.14;
				hist3->Fill(aa2);
				//	cout<<" "<<VX1<<" "<<VY1<<" "<<VZ1<<" "<<reflectorid<< " "<<tubeid<<endl;
				//	cout <<" "<<reflectorid<<" "<<tubeid<<endl;
	  		      }
	 		    if(tem1!=tem)
	  		      {
	 			tem1=tem;
				pr2dvector[j].SetCoordinates(directionx, directiony, directionz);
				vr2xector[j].SetCoordinates(VX1, VY1, VZ1);
				double s2 = acos((pr2dvector[j].Dot(vr2xector[j])));
				float aa2 = s2*180/3.14;
				hist3->Fill(aa2);
				//cout<<" "<<VX1<<" "<<VY1<<" "<<VZ1<<" "<<reflectorid<< " "<<tubeid<<endl;
				//cout <<" "<<reflectorid<<" "<<tubeid<<endl;	
	   		      }
			      }
		 }
		     }
		 int r, r1;
		 //	if (reflectortrackid == trackid)
		 // {
	  		    // if (ref == 0)
			    //   // {
			    //   //r = trackid;
			    // 	//	cout <<" "<<reflectorid<<" "<<tubeid<<endl;	
			      
	  		    // // if(j==0)
	  		    //   {
	  		    // 	//r1=r;
			    // 	pr3dvector[i].SetCoordinates(directionx, directiony, directionz);
			    // 	vr3xector[i].SetCoordinates(VX1, VY1, VZ1);
			    // 	double s3 = acos((pr3dvector[i].Dot(vr2xector[i])));
			    // 	float aa3 = s3*180/3.14;
			    // 	hist4->Fill(aa3);
			    // 	//	cout<<" "<<VX1<<" "<<VY1<<" "<<VZ1<<" "<<reflectorid<< " "<<tubeid<<endl;
			    // 	//	cout <<" "<<reflectorid<<" "<<tubeid<<endl;
	  		    //   }
	 		    // if(r1!=r)
	  		  //     {
	 		  // 	r1=r;
			  // 	pr3dvector[j].SetCoordinates(directionx, directiony, directionz);
			  // 	vr3xector[j].SetCoordinates(VX1, VY1, VZ1);
			  // 	double s3 = acos((pr3dvector[j].Dot(vr3xector[j])));
			  // 	float aa3 = s3*180/3.14;
			  // 	hist4->Fill(aa3);
			  // 	//cout<<" "<<VX1<<" "<<VY1<<" "<<VZ1<<" "<<reflectorid<< " "<<tubeid<<endl;
			  // 	//cout <<" "<<reflectorid<<" "<<tubeid<<endl;	
	   		  //     }
	  		  // }
			    // }
		  
		     
	      }
	  }

		//	const int ref = wcsimroottrigger->GetNreflectorhits();  
		// 	for (int j=0; j<ref; j++)
	        // {
		   
		// WCSimRootReflectorHit *rhit = (WCSimRootReflectorHit*)
		// (wcsimroottrigger->GetReflectorHits()->At(j));
		//int reflectorid = rhit -> GetReflectorID();
		// int reflectortrackid = rhit -> GetRefTrackID();
		//double x_ref = rhit->GetReflectorPos(0)*0.1;
		//double y_ref = rhit->GetReflectorPos(1)*0.1;
		//double z_ref = rhit->GetReflectorPos(2)*0.1;	  
		//double theta_r =atan(z_ref/x_ref)*180/3.14159;
		// double thetaa_r = 0;	 
		//if (z_ref<0 && x_ref>0) {
		//  thetaa_r = theta_r+360;
		//}
		//else if (x_ref<0 && z_ref>0){
		//  thetaa_r = theta_r+180;
		// }
		//else if (x_ref<0 && z_ref<0){
		// thetaa_r = theta_r + 180;
		//}
		//else if (x_ref>0 && z_ref>0){
		//  thetaa_r = theta_r;
		// }							
		   //  int iHR = chit -> GetIsHitReflector();
	  // 	    WCSimRootPMT pmt = wcsimrootgeom ->GetPMT(tubeid-1);
	  // 	    double X = pmt.GetPosition(0);
	  // 	    double Y = pmt.GetPosition(1);
	  // 	    double Z = pmt.GetPosition(2);
	  // 	    double theta =atan(Z/X)*180/3.14159;
	  // 	    double thetaa = 0;
	  // 	    if (Z<0 && X>0) {
	  // 	      thetaa = theta+360;
	  // 	    }
	  // 	    else if (X<0 && Z>0){
	  // 	      thetaa = theta+180;
	  // 	    }
	  // 	    else if (X<0 && Z<0){
	  // 	      thetaa = theta + 180;
	  // 	    }
	  // 	    else if (X>0 && Z>0){
	  // 	      thetaa = theta;
	  // 	    }
	  // 	    int ch, ch1;
	  // 	    //=======================Cherenkovhit========================================//
		    
	  // 	    if (Y >= -280.0 && Y <= 280.0) //barrel
	  // 	      //  if (Y >= 280.0) //topcap
	  // 	      //if (Y <= -280.0) //bottom
	  // 	      {
	  // 		ch = trackid;
	  // 		if(j==0)
	  // 		  {
	  // 		    ch1 = ch;
	  // 		    double charge1 =chit -> GetTotalPe(1); 
	  // 		    hist1->Fill(thetaa, Y, charge1);
	  // 		  }
	  // 		if(ch1 != ch)
	  // 		  {
	  // 		    ch1 = ch;
	  // 		    double charge1 =chit -> GetTotalPe(1); 
	  // 		    hist1->Fill(thetaa, Y, charge1);
	  // 		  }
	  // 	      }
	  // 	    //=================reflectorhit direct light all counts=========================================//
		    
	  // 	    if (Y >= -280.0 && Y <= 280.0) //barrel
	  // 	      // if (Y >= 280.0) //top
	  // 	      //if (Y <= -280.0) //bottom
	  // 	      {
	  // 		if (reflectortrackid == trackid)
	  // 		  {
	  // 		    if (reflectorid == tubeid)
	  // 		      {
	  // 			double charge =chit -> GetTotalPe(1); 
	  // 			hist2->Fill(thetaa_r, y_ref, charge);
	  // 		      }
	  // 		  }
	  // 	      }
	  // 	    int temp, temp1;
		    
	  // 	    if (Y >= -280.0 && Y <= 280.0) //barrel
	  // 	    // if (Y >= 280.0) //top
	  // 	      //if (Y <= -280.0) //bottom
	  // 	      {
	  // 		if (reflectortrackid == trackid)
	  // 		  {
	  // 		    if (reflectorid != tubeid)
	  // 		      {
	  // 			temp = trackid;
	  // 			double charge =chit -> GetTotalPe(1); 
	  // 			hist3->Fill(thetaa_r, y_ref);
	  // 		      }
	  // 		    if(j==0)
	  // 		      {
	  // 			temp1=temp;
	  // 			double charge1 =chit -> GetTotalPe(1); 
	  // 			hist4->Fill(thetaa_r, y_ref, charge1);
	  // 		      }
	  // 		    if(temp1!=temp)
	  // 		      {
	  // 			temp1=temp;
	  // 			double charge1 =chit -> GetTotalPe(1); 
	  // 			hist4->Fill(thetaa_r, y_ref, charge1);
	  // 		      }
	  // 		  }
	  // 	      }
		    
	  // 	 }
		
	  //     }
	  // }
	
	
	
	
	
	cout << "plotted cd 1" << endl;
	c1 -> cd(1);
	hist1-> SetXTitle("Theta");
	//	hist1->SetYTitle("Y coordinate in cm");
	hist1->Draw("colz");
		cout << "plotted cd 2" << endl;
	c1 -> cd(2);
	hist2-> SetXTitle("theta");
	//	hist2->SetYTitle("Y coordinate in cm");
	hist2->SetTitleOffset(1.5);
	hist2->Draw("colz");
	c1 -> cd(3);
	hist3-> SetXTitle("theta");
	//	hist3->SetYTitle("Y coordinate in cm");
	hist3->SetTitleOffset(1.5);
	hist3->Draw("colz");
       c1 -> cd(4);
        hist4-> SetXTitle("theta");
	// hist4->SetYTitle("Y coordinate in cm");
        hist4->SetTitleOffset(1.5);
        hist4->Draw("colz");
	return;
}
int main(int argc, char** argv) {
  TApplication app("ROOT Application", &argc, argv);
  reflectorhit(app.Argc(), app.Argv());
  app.Run();
  
  // app.Terminate(); // still doesn't quit
  
  return 0;
}
