int monitorBAC( int lastrun = 6228 )
{

  //==== setting =========
  int firstrun =  6176;

  //==== setting for font =========
  int fontid = 62; // 132 for article, 62 for slide
  int italic = 12; // font of italic. 12 for normal, 32 for bold.
  gStyle->SetStatFont(fontid);
  gStyle->SetLabelFont(fontid, "XYZ");
  gStyle->SetLabelFont(fontid, "");
  gStyle->SetTitleFont(fontid, "XYZ");
  gStyle->SetTitleFont(fontid, "");
  gStyle->SetTitleSize(0.045, "XYZ");
  gStyle->SetTextFont(fontid);
  gStyle->SetLegendFont(fontid);
  gStyle->SetLabelSize(0.035, "XYZ");
  gStyle->SetPadGridX(1); // display vertical grid lines ( 0 is not to display )
  gStyle->SetPadGridY(1);

  std::vector<double> runNo;
  std::vector<double> BACperSpill; // BAC divided Spill Num

  for( int irun = firstrun; irun <= lastrun; irun++ ){
    //===== open file =======
    std::string fname = Form("../scaler_2021may/scaler_%05d.txt", irun);
    std::ifstream ifs(fname);
    if( !ifs.is_open() ){
      std::cerr << "cannot open file : "
		<< fname << std::endl;
      continue;
    }

    //==== std::cout << count ========
    std::string tmp;
    std::string key;
    int count;
    int BAC;
    int Spill;
    while( getline(ifs, tmp) ){
      std::stringstream stream;
      stream << tmp;
      stream >> key >> count;
      if( key == "BAC" ){
	BAC = count;
	std::cout << " run No.: " << std::setw(4) << irun
		  << ",  BAC: "   << std::setw(10) << BAC;
      }
      else if( key == "Spill" ){
	Spill = count;
	std::cout << ",  Spill: " << std::setw(3) << Spill
		  << std::endl;
	if( Spill > 500 ){
	  runNo.push_back(irun);
	  BACperSpill.push_back((double)BAC/Spill);
	}
	break;
      }
    }
  }

  TCanvas *cg = new TCanvas("cg", "cg");

  TGraph *g_BAC = new TGraph(runNo.size(), &runNo[0], &BACperSpill[0]);
  g_BAC->SetTitle("BAC / Spill {Spill > 500}");
  g_BAC->GetXaxis()->SetTitle("Run#");
  g_BAC->SetLineColor(kRed);
  g_BAC->Draw();

  return 0;
}
