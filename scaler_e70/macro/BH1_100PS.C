int BH1_100PS()
{

  int runfirst =  6084;
  int runlast  =  6156;

  std::vector<double> runNo;
  std::vector<double> BH1perSpill; // BH1 OR divided Spill Num
  std::vector<double> BH1_100PSperSpill; // BH1-1/100PS divided Spill Num

  for( int irun = runfirst; irun <= runlast; irun++ ){
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
    int BH1;
    int BH1_100PS;
    int Spill;
    while( getline(ifs, tmp) ){
      std::stringstream stream;
      stream << tmp;
      stream >> key >> count;
      if( key == "BH1" ){
	BH1 = count;
	std::cout << " run No.: " << std::setw(4) << irun
		  << ",  BH1: "    << std::setw(10) << BH1;
      }
      if( key == "BH1-1/100-PS" ){
	BH1_100PS = count;
	std::cout  << ",  BH1_100PS: "  << std::setw(8) << BH1_100PS;
      }
      else if( key == "Spill" ){
	Spill = count;
	std::cout << ",  Spill: " << std::setw(3) << Spill
		  << std::endl;
	if( Spill > 500 ){
	  runNo.push_back(irun);
	  BH1perSpill.push_back((double)BH1/Spill);
	  BH1_100PSperSpill.push_back((double)BH1_100PS/Spill);
	}
	break;
      }
    }
  }

  TCanvas *cg = new TCanvas("cg", "cg");
  cg->Divide(2, 1);

  cg->cd(1);
  TGraph *g_BH1 = new TGraph(runNo.size(), &runNo[0], &BH1perSpill[0]);
  g_BH1->SetTitle("BH1 / Spill @ Kaon 530k/spill {Spill > 500}");
  g_BH1->GetXaxis()->SetTitle("Run#");
  g_BH1->SetLineColor(kRed);
  g_BH1->Draw();

  cg->cd(2);
  TGraph *g_BH1100PS = new TGraph(runNo.size(), &runNo[0], &BH1_100PSperSpill[0]);
  g_BH1100PS->SetTitle("BH1-1/100PS / Spill @ Kaon 530k/spill {Spill > 500}");
  g_BH1100PS->GetXaxis()->SetTitle("Run#");
  g_BH1100PS->SetLineColor(kRed);
  g_BH1100PS->Draw();

  return 0;
}
