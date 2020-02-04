//#############################################
//
// Kill cells in SN-IEgenerator root simulation
// Reads the output from the SN-IEgenerator and 
// writes a file without hits on killed cells
//  
// P. Franchini 2020
//
//#############################################

#include "TCanvas.h"
#include "TFile.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TTree.h"
#include "TLine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::vector<double>* dirx;
std::vector<double>* diry;
std::vector<double>* dirz;
std::vector<double>* pointx;
std::vector<double>* pointy;
std::vector<double>* pointz;
std::vector<double>* breakpointx;
std::vector<double>* breakpointy;
std::vector<double>* bpangle;
std::vector<double>* radius;
std::vector<double>* wirex;
std::vector<double>* wirey;
std::vector<double>* wirez;
std::vector<int>* grid_id;
std::vector<int>* grid_side;
std::vector<int>* grid_layer;
std::vector<int>* grid_column;
std::vector<int>* break_layer;
std::vector<int>* charge;
std::vector<int>* calo_id;
std::vector<int>* calo_type;
std::vector<int>* calo_side;
std::vector<int>* calo_wall;
std::vector<int>* calo_column;
std::vector<int>* calo_row;
// have been declared global because of memory issue in GetEntry()

static void show_usage(std::string name)
{
  std::cerr << "Usage: ./dead_cells <option(s)>\n"
	    << "Options:\n"
	    << "\t-i,--input\t\tInput root file\n"
	    << "\t-o,--output\t\tOutput root file\n"
	    << "\t-n,--num-dead-cells\tNumber of dead cells\n"
	    << "\t-d,--dead-cells-file\tFile with list of dead cells (side layer column)"
	    << std::endl;
}

// Tracker dimensions:
const double d = 44.0; 
const double offsetx = 53.0;
const double offsety = -2464.0;


int main(int argc, char* argv[]){

  //if (argc < 4) {
  //  show_usage(argv[0]);
  //  return 1;
  //}

  //######################################################################################################################

  float do_radius = true; // If `true` keeps the hit but set the radius to zero, if `false` removes the whole tracker hit
  
  //######################################################################################################################
  
  std::string input, output, file;
  int N_dead_cells=0;
  
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      return 0;
    }
    if ((arg == "-i") || (arg == "--input"))
      input = argv[i+1];
    if ((arg == "-o") || (arg == "--output"))
      output = argv[i+1];
    if ((arg == "-n") || (arg == "--num-dead-cells"))
      N_dead_cells = atoi(argv[i+1]);
    if ((arg == "-d") || (arg == "--dead-cells-file"))
      file = argv[i+1];

  }
  
  std::cout << "Input file: " << input << std::endl;
  std::cout << "Output file: " << output << std::endl;
  if (!file.empty()) std::cout << "Dead cells input file: " << file << std::endl;

  // Input and output files from the arguments
  TFile *f1 = new TFile(input.c_str());
  TTree *t1 = (TTree*)f1->Get("hit_tree");

  TCanvas *c1 = new TCanvas("c1","Projections",1800,600);
  TCanvas *c2 = new TCanvas("c2","Tracker dead cells",900,600);
  gStyle->SetOptStat(0);
  c1->Divide(3,1);

  Int_t nbins=1000;
  TH2F *top_view   = new TH2F("top_view","Top view", nbins/6, -500, 500, nbins, -3000, 3000);
  TH2F *side_view  = new TH2F("side_view","Side view", nbins/6, -500, 500, nbins, -1600, 1600);
  TH2F *front_view = new TH2F("front_view","Front view", nbins/6, -3000, 3000, nbins, -1600, 1600);

  TH2F *tracker   = new TH2F("tracker","Tracker Dead Cells (Top view)", 113, 0, 112, 18, 0, 17);

  t1->SetBranchAddress("dirx",&dirx);
  t1->SetBranchAddress("diry",&diry);
  t1->SetBranchAddress("dirz",&dirz);
  t1->SetBranchAddress("pointx",&pointx);
  t1->SetBranchAddress("pointy",&pointy);
  t1->SetBranchAddress("pointz",&pointz);
  t1->SetBranchAddress("breakpointx",&breakpointx);
  t1->SetBranchAddress("breakpointy",&breakpointy);
  t1->SetBranchAddress("bpangle",&bpangle);
  t1->SetBranchAddress("radius",&radius);
  t1->SetBranchAddress("wirex",&wirex);
  t1->SetBranchAddress("wirey",&wirey);
  t1->SetBranchAddress("wirez",&wirez);
  t1->SetBranchAddress("grid_id",&grid_id);
  t1->SetBranchAddress("grid_side",&grid_side);
  t1->SetBranchAddress("grid_layer",&grid_layer);
  t1->SetBranchAddress("grid_column",&grid_column);
  t1->SetBranchAddress("break_layer",&break_layer);
  t1->SetBranchAddress("charge",&charge);
  t1->SetBranchAddress("calo_id",&calo_id);
  t1->SetBranchAddress("calo_type",&calo_type);
  t1->SetBranchAddress("calo_side",&calo_side);
  t1->SetBranchAddress("calo_wall",&calo_wall);
  t1->SetBranchAddress("calo_column",&calo_column);
  t1->SetBranchAddress("calo_row",&calo_row);

  Long64_t hits = 0;
  Long64_t killed = 0;

  bool kill = false;

  // Define a matrix (side, layer, column) of dead cells rolling dices
  int dead_cells[N_dead_cells][3];
  
  bool isNew = false;

  // Dead cells (from file or from random selection):
  if (!file.empty()){
    // Dead cells from file:
    std::cout << "Reading dead cells from file..." << std::endl;
    std::ifstream infile(file);   // File for list of dead cells
    Long64_t i=0;
    int side, layer, column;
    while (infile >> side >> layer >> column){
      dead_cells[i][0]=side;  
      dead_cells[i][1]=layer;
      dead_cells[i][2]=column;
      i++;
    }
    N_dead_cells=i;
    // print list of dead cells
    //  for (Long64_t i=0; i<N_dead_cells; i++)
    //std::cout << dead_cells[i][0] << "-" << dead_cells[i][1] << "-" << dead_cells[i][2] << std::endl;
  }
  else {
    // Random dead cells:
    std::cout << "Generating random dead cells..." << std::endl;
    for (Long64_t i=0; i<N_dead_cells; i++){
      // need to check if the dead_cell is not already in the list
      isNew = false;
      while (!isNew) {  // try a random cell
	dead_cells[i][0]=rand() % 2;    // side
	dead_cells[i][1]=rand() % 9;    // layer
	dead_cells[i][2]=rand() % 113;  // column
	isNew = true; // assume is new
	for (Long64_t j=0; j<i; j++){  // look if is alredy in the  matrix
	  if ( (dead_cells[i][0]==dead_cells[j][0]) && (dead_cells[i][1]==dead_cells[j][1]) && (dead_cells[i][2]==dead_cells[j][2]) ) {
	    isNew = false; // is already in the matrix so will try again 
	    break;
	  }
	}
      }
    }
  }

  // Fill TH2 with dead cells positions
  std::cout << "Number of dead cells: " << N_dead_cells << std::endl;
  for (Long64_t i=0; i<N_dead_cells; i++)
    if (dead_cells[i][0]==0)
      tracker->Fill(dead_cells[i][2],8-dead_cells[i][1]);
    else
      tracker->Fill(dead_cells[i][2],dead_cells[i][1]+9);
      
  // Create a new file with a new tree
  std::vector<double>* dirx2 = new std::vector<double>;
  std::vector<double>* diry2 = new std::vector<double>;
  std::vector<double>* dirz2 = new std::vector<double>;
  std::vector<double>* pointx2 = new std::vector<double>;
  std::vector<double>* pointy2 = new std::vector<double>;
  std::vector<double>* pointz2 = new std::vector<double>;
  std::vector<double>* breakpointx2 = new std::vector<double>;
  std::vector<double>* breakpointy2 = new std::vector<double>;
  std::vector<double>* bpangle2 = new std::vector<double>;
  std::vector<double>* radius2 = new std::vector<double>;
  std::vector<double>* wirex2 = new std::vector<double>;
  std::vector<double>* wirey2 = new std::vector<double>;
  std::vector<double>* wirez2 = new std::vector<double>;
  std::vector<int>* grid_id2 = new std::vector<int>;
  std::vector<int>* grid_side2 = new std::vector<int>;
  std::vector<int>* grid_layer2 = new std::vector<int>;
  std::vector<int>* grid_column2 = new std::vector<int>;
  std::vector<int>* break_layer2 = new std::vector<int>;
  std::vector<int>* charge2 = new std::vector<int>;
  std::vector<int>* calo_id2 = new std::vector<int>;
  std::vector<int>* calo_type2 = new std::vector<int>;
  std::vector<int>* calo_side2 = new std::vector<int>;
  std::vector<int>* calo_wall2 = new std::vector<int>;
  std::vector<int>* calo_column2 = new std::vector<int>;
  std::vector<int>* calo_row2 = new std::vector<int>;

  TFile *f2 = new TFile(output.c_str(),"recreate");
  TTree t2("hit_tree","Hit Data");

  t2.Branch("dirx",&dirx2);
  t2.Branch("diry",&diry2);
  t2.Branch("dirz",&dirz2);
  t2.Branch("pointx",&pointx2);
  t2.Branch("pointy",&pointy2);
  t2.Branch("pointz",&pointz2);
  t2.Branch("breakpointx",&breakpointx2);
  t2.Branch("breakpointy",&breakpointy2);
  t2.Branch("bpangle",&bpangle2);
  t2.Branch("radius",&radius2);
  t2.Branch("wirex",&wirex2);
  t2.Branch("wirey",&wirey2);
  t2.Branch("wirez",&wirez2);
  t2.Branch("grid_id",&grid_id2);
  t2.Branch("grid_side",&grid_side2);
  t2.Branch("grid_layer",&grid_layer2);
  t2.Branch("grid_column",&grid_column2);
  t2.Branch("break_layer",&break_layer2);
  t2.Branch("charge",&charge2);
  t2.Branch("calo_id",&calo_id2);
  t2.Branch("calo_type",&calo_type2);
  t2.Branch("calo_side",&calo_side2);
  t2.Branch("calo_wall",&calo_wall2);
  t2.Branch("calo_column",&calo_column2);
  t2.Branch("calo_row",&calo_row2);

  // Start looping over all the events
  Long64_t nentries = t1->GetEntries();
  for (Long64_t j=0;j<nentries;j++) {

    // flush the variables for the new entry in the output file
    wirex2->clear();
    wirey2->clear();
    wirez2->clear();
    grid_side2->clear();
    grid_layer2->clear();
    grid_column2->clear();
    grid_id2->clear();
    radius2->clear();
    dirx2->clear();
    diry2->clear();
    dirz2->clear();
    pointx2->clear();
    pointy2->clear();
    pointz2->clear();
    breakpointx2->clear();
    breakpointy2->clear();
    bpangle2->clear();
    break_layer2->clear();
    charge2->clear();
    calo_id2->clear();
    calo_type2->clear();
    calo_side2->clear();
    calo_wall2->clear();
    calo_column2->clear();
    calo_row2->clear();
    
    t1->GetEntry(j);
    
    // Loops over the hits of one event. The size is the same for the 8 vectors wire*, grid_* and radius
    for (unsigned int i=0; i < wirex->size(); i++){
      
      kill=false;
      hits++;
      
      // Find if this hit of this event is on any dead cell or not
      for (Long64_t k=0; k<N_dead_cells; k++){
	if ( ((*grid_side)[i]==dead_cells[k][0]) && ((*grid_layer)[i]==dead_cells[k][1]) && ((*grid_column)[i]==dead_cells[k][2])  ){  // is dead
	  /*	  std::cout << (*wirex)[i] << "," << (*wirey)[i] << "," << (*wirez)[i] << std::endl;
	  std::cout << (*grid_side)[i] << "," << (*grid_layer)[i] << "," << (*grid_column)[i] << std::endl;
	  if ((*grid_side)[i])
	    std::cout << (*grid_layer)[i]*d+offsetx << "," << (*grid_column)[i]*d+offsety << std::endl << std::endl;
	  else
	    std::cout << -((*grid_layer)[i]*d+offsetx) << "," << (*grid_column)[i]*d+offsety << std::endl << std::endl;
	  */
	  
	  kill=true;
	  break; // ends loop since the hit is already on one dead cell
	}
      } // end loop in the killing procedure
      
      if (!kill){ // keep everything
	
	// populate plots with survived events
	top_view->Fill((*wirex)[i],(*wirey)[i]);
	side_view->Fill((*wirex)[i],(*wirez)[i]);
	front_view->Fill((*wirey)[i],(*wirez)[i]);
	
	// write to the new tree variables the tracker cell data of the survived events
	wirex2->push_back((*wirex)[i]);
	wirey2->push_back((*wirey)[i]);
	wirez2->push_back((*wirez)[i]);
	grid_side2->push_back((*grid_side)[i]);
	grid_layer2->push_back((*grid_layer)[i]);
	grid_column2->push_back((*grid_column)[i]);
	grid_id2->push_back((*grid_id)[i]);
	radius2->push_back((*radius)[i]);

      }
      else
	if (do_radius){  // keep the hit and reduce the radius to 0
	  wirex2->push_back((*wirex)[i]);
	  wirey2->push_back((*wirey)[i]);
	  wirez2->push_back((*wirez)[i]);
	  grid_side2->push_back((*grid_side)[i]);
	  grid_layer2->push_back((*grid_layer)[i]);
	  grid_column2->push_back((*grid_column)[i]);
	  grid_id2->push_back((*grid_id)[i]);
	  radius2->push_back(0);  // radius to 0
	}
	else // removes the whole tracker hit
	  killed++;
    }

    // clone everything else that is not a tracker cell data
    dirx2=dirx;
    diry2=diry;
    dirz2=dirz;
    pointx2=pointx;
    pointy2=pointy;
    pointz2=pointz;
    breakpointx2=breakpointx;
    breakpointy2=breakpointy;
    bpangle2=bpangle;
    break_layer2=break_layer;
    charge2=charge;
    calo_id2=calo_id;
    calo_type2=calo_type;
    calo_side2=calo_side;
    calo_wall2=calo_wall;
    calo_column2=calo_column;
    calo_row2=calo_row;

    // fill the hacked event to the new tree
    t2.Fill();
  
  }

  top_view->GetXaxis()->SetTitle("Wire X [mm]");
  top_view->GetYaxis()->SetTitle("Wire Y [mm]");
  side_view->GetXaxis()->SetTitle("Wire X [mm]");
  side_view->GetYaxis()->SetTitle("Wire Z [mm]");
  front_view->GetXaxis()->SetTitle("Wire Y [mm]");
  front_view->GetYaxis()->SetTitle("Wire Z [mm]");

  c1->cd(1);
  gPad->SetGrid();
  top_view->Draw("colz");
  c1->cd(2);
  gPad->SetGrid();
  side_view->Draw("colz");
  c1->cd(3);
  gPad->SetGrid();
  front_view->Draw("colz");
  gPad->Modified(); gPad->Update();

  c2->cd();
  tracker->GetXaxis()->SetTitle("Column");
  tracker->GetYaxis()->SetTitle("Layer");
  TLine *l=new TLine(0,8.5,113,8.5);
  l->SetLineColor(kBlue);
  l->SetLineWidth(3);
  tracker->Draw("colz");
  l->Draw("same");
  
  t2.Write();
  //f2->Close();  

  std::cout << "Total number of hits: " << hits << std::endl;
  if (!do_radius)
    std::cout << "Killed pixels: " << killed << std::endl;
  else
    std::cout << "Radius set to zero for dead cells" << std::endl;

  c1->SaveAs("tracker.png");
  c2->SaveAs("tracker_DC.png");
  //system("display tracker*.png");

  //f1->Close();
  //f2->Close();

  delete t1;
  delete f1;
  delete f2;

  //  delete t2;

  return 0;

}

