#include "WCSimRunActionMessenger.hh"

#include "WCSimRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"

WCSimRunActionMessenger::WCSimRunActionMessenger(WCSimRunAction* WCSimRA)
:WCSimRun(WCSimRA)
{ 
  WCSimIODir = new G4UIdirectory("/WCSimIO/");
  WCSimIODir->SetGuidance("Commands to select I/O options");

  RootFile = new G4UIcmdWithAString("/WCSimIO/RootFile",this);
  RootFile->SetGuidance("Set the root file name");
  RootFile->SetGuidance("Enter the name of the output ROOT file");
  RootFile->SetParameterName("RootFileName",true);
  RootFile->SetDefaultValue("wcsim.root");

  WriteDefaultRootFile = new G4UIcmdWithABool("/WCSimIO/WriteDefaultRootFile",this);
  WriteDefaultRootFile->SetGuidance("Do you want to write out the standard ROOT file format. The new FLAT one you get for free.");
  WriteDefaultRootFile->SetParameterName("WriteDefaultFile",true);
  WriteDefaultRootFile->SetDefaultValue(false);  //ToDo: memo: default = FALSE !! Move to novis.mac!

}

WCSimRunActionMessenger::~WCSimRunActionMessenger()
{
  delete WriteDefaultRootFile;
  delete RootFile;
  delete WCSimIODir;
}

void WCSimRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if ( command == RootFile)
    {
      WCSimRun->SetRootFileName(newValue);
      G4cout << "Output ROOT file set to " << newValue << G4endl;
    }
  else if (command == WriteDefaultRootFile )
    {
      WCSimRun->SetOptionalRootFile(WriteDefaultRootFile->GetNewBoolValue(newValue));
      G4cout << "You chose to write out the standard ROOT file: " << WriteDefaultRootFile->GetNewBoolValue(newValue) << G4endl;
    }
}
