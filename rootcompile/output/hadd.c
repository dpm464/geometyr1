#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
 
TList *FileList;
TFile *Target;
 
void MergeRootfile( TDirectory *target, TList *sourcelist );
 
 
void hadd() {
   // Prepare the files to me merged
   if(gSystem->AccessPathName("hsimple1.root")) {
     gSystem->CopyFile("tt1.root", "hsimple1.root");
     gSystem->CopyFile("tt2.root", "hsimple2.root");
     gSystem->CopyFile("tt3.root", "hsimple3.root");
     // gSystem->CopyFile("rt4.root", "hsimple4.root");
     // gSystem->CopyFile("rt5.root", "hsimple5.root");
     //gSystem->CopyFile("rt6.root", "hsimple6.root");
     // gSystem->CopyFile("rt7.root", "hsimple7.root");
     // gSystem->CopyFile("rt8.root", "hsimple8.root");
     // gSystem->CopyFile("rt9.root", "hsimple9.root");
     //gSystem->CopyFile("rt10.root", "hsimple10.root");
     // gSystem->CopyFile("rt11.root", "hsimple11.root");
     // gSystem->CopyFile("rt12.root", "hsimple12.root");
     //gSystem->CopyFile("rt13.root", "hsimple13.root");
     //gSystem->CopyFile("rt14.root", "hsimple14.root");
     // gSystem->CopyFile("rt15.root", "hsimple15.root");
       //gSystem->CopyFile("ti16.root", "hsimple16.root");
       //gSystem->CopyFile("ti17.root", "hsimple17.root");
       // gSystem->CopyFile("ti18.root", "hsimple18.root");
       //	gSystem->CopyFile("ti19.root", "hsimple19.root");
       // gSystem->CopyFile("ti20.root", "hsimple20.root");
   }
 
   // in an interactive ROOT session, edit the file names
   // Target and FileList, then
   // root > .L hadd.C
   // root > hadd()
 
   Target = TFile::Open( "toptotalfinal.root", "RECREATE" );
 
   FileList = new TList();
   FileList->Add( TFile::Open("hsimple1.root") );
FileList->Add( TFile::Open("hsimple2.root") );
FileList->Add( TFile::Open("hsimple3.root") );
//FileList->Add( TFile::Open("hsimple4.root") );
//FileList->Add( TFile::Open("hsimple5.root") );
//FileList->Add( TFile::Open("hsimple6.root") );
//FileList->Add( TFile::Open("hsimple7.root") );
//FileList->Add( TFile::Open("hsimple8.root") );
//FileList->Add( TFile::Open("hsimple9.root") );
//FileList->Add( TFile::Open("hsimple10.root") );
//FileList->Add( TFile::Open("hsimple11.root") );
//FileList->Add( TFile::Open("hsimple12.root") );
//FileList->Add( TFile::Open("hsimple13.root") );
//FileList->Add( TFile::Open("hsimple14.root") );
//FileList->Add( TFile::Open("hsimple15.root") );
//FileList->Add( TFile::Open("hsimple16.root") );
//FileList->Add( TFile::Open("hsimple17.root") );
//FileList->Add( TFile::Open("hsimple18.root") );
//FileList->Add( TFile::Open("hsimple19.root") );
//FileList->Add( TFile::Open("hsimple20.root") );


   MergeRootfile( Target, FileList );
 
}
 
void MergeRootfile( TDirectory *target, TList *sourcelist ) {
 
   //  cout << "Target path: " << target->GetPath() << endl;
   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove( 0, 2 );
 
   TFile *first_source = (TFile*)sourcelist->First();
   first_source->cd( path );
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
 
   // loop over all keys in this directory
   TChain *globChain = 0;
   TIter nextkey( current_sourcedir->GetListOfKeys() );
   TKey *key, *oldkey=0;
   while ( (key = (TKey*)nextkey())) {
 
      //keep only the highest cycle number for each key
      if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
 
      // read object from first source file
      first_source->cd( path );
      TObject *obj = key->ReadObj();
 
      if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
         // descendant of TH1 -> merge it
 
         //      cout << "Merging histogram " << obj->GetName() << endl;
         TH1 *h1 = (TH1*)obj;
 
         // loop over all source files and add the content of the
         // correspondant histogram to the one pointed to by "h1"
         TFile *nextsource = (TFile*)sourcelist->After( first_source );
         while ( nextsource ) {
 
            // make sure we are at the correct directory level by cd'ing to path
            nextsource->cd( path );
            TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
            if (key2) {
               TH1 *h2 = (TH1*)key2->ReadObj();
               h1->Add( h2 );
               delete h2;
            }
 
            nextsource = (TFile*)sourcelist->After( nextsource );
         }
      }
      else if ( obj->IsA()->InheritsFrom( TTree::Class() ) ) {
 
         // loop over all source files create a chain of Trees "globChain"
         const char* obj_name= obj->GetName();
 
         globChain = new TChain(obj_name);
         globChain->Add(first_source->GetName());
         TFile *nextsource = (TFile*)sourcelist->After( first_source );
         //      const char* file_name = nextsource->GetName();
         // cout << "file name  " << file_name << endl;
         while ( nextsource ) {
 
            globChain->Add(nextsource->GetName());
            nextsource = (TFile*)sourcelist->After( nextsource );
         }
 
      } else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
         // it's a subdirectory
 
         cout << "Found subdirectory " << obj->GetName() << endl;
 
         // create a new subdir of same name and title in the target file
         target->cd();
         TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
 
         // newdir is now the starting point of another round of merging
         // newdir still knows its depth within the target file via
         // GetPath(), so we can still figure out where we are in the recursion
         MergeRootfile( newdir, sourcelist );
 
      } else {
 
         // object is of no type that we know or can handle
         cout << "Unknown object type, name: "
         << obj->GetName() << " title: " << obj->GetTitle() << endl;
      }
 
      // now write the merged histogram (which is "in" obj) to the target file
      // note that this will just store obj in the current directory level,
      // which is not persistent until the complete directory itself is stored
      // by "target->Write()" below
      if ( obj ) {
         target->cd();
 
         //!!if the object is a tree, it is stored in globChain...
         if(obj->IsA()->InheritsFrom( TTree::Class() ))
            globChain->Merge(target->GetFile(),0,"keep");
         else
            obj->Write( key->GetName() );
      }
 
   } // while ( ( TKey *key = (TKey*)nextkey() ) )
 
   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);
}
