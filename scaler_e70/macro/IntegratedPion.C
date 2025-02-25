int IntegratedPion( int lastrun = 6160 )
{

  //==== setting =========
  TDatime StartTime(2024, 4, 26, 17, 0, 0);
  TDatime EndTime(2024, 4, 26, 23, 59, 0);
  int goalPi   =   3;
  int firstrun =  71374;

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
  gStyle->SetLabelOffset(0.035, "X");
  gStyle->SetPadGridX(1); // display vertical grid lines ( 0 is not to display )
  gStyle->SetPadGridY(1);

  int TimeOffset = -788918400;


  //==== get last run No. =================

  const char* cmd = "ls -ltr ../scaler_2024may/ | tail -n 1 > tmp.txt";
  system(cmd);
  std::ifstream ifs_lastrun("tmp.txt");
  if( !ifs_lastrun.is_open() ){
    std::cerr << "cannot open file : tmp.txt" << std::endl;
    return -1;
  }
  TString line;
  if( ifs_lastrun.good() ) line.ReadLine(ifs_lastrun);
  Ssiz_t pos = line.Last('.');
  TString tmp = line(pos-5, pos);
  lastrun = tmp.Atoi();
  std::cout << "lastrun is Run#" << lastrun << std::endl;
  sleep(1);

  //===== Integrated Pion ========
  std::vector<double> time;
  std::vector<double> PionSum;

  double KSum = 0;
  for( int irun = firstrun; irun <= lastrun; irun++ ){
    //===== open file =======
    std::string fname = Form("../scaler_2024may/scaler_%05d.txt", irun);
    std::ifstream ifs(fname);
    if( !ifs.is_open() ){
      std::cerr << "cannot open file : "
  		<< fname << std::endl;
      continue;
    }

    //==== std::cout << count ========
    std::string tmp;
    std::string key;
    std::map <TString, Int_t> month_map = { {"Jan", 1},
					    {"Feb", 2},
					    {"Mar", 3},
					    {"Apr", 4},
					    {"May", 5},
					    {"Jun", 6},
					    {"Jul", 7},
					    {"Aug", 8},
					    {"Sep", 9},
					    {"Oct", 10},
					    {"Nov", 11},
					    {"Dec", 12} };
    int year, month, day;
    int hour, min, sec;
    int count;
    int KBeam;
    double KBeamG;
    while( getline(ifs, tmp) ){
      if( tmp.find("RUN") != std::string::npos ){
  	int DatePos = tmp.rfind("  -  ") + 9;
  	month = month_map[tmp.substr(DatePos, 3)];
  	day   = stoi(tmp.substr(DatePos+4, 2));
  	hour  = stoi(tmp.substr(DatePos+7, 2));
  	min   = stoi(tmp.substr(DatePos+10, 2));
  	sec   = stoi(tmp.substr(DatePos+13, 2));
  	year  = stoi(tmp.substr(DatePos+16, 4));
  	std::cout << std::setfill('0') << std::right
  		  << "[" << std::setw(4) << year
  		  << "-" << std::setw(2) << month
  		  << "-" << std::setw(2) << day
  		  << " " << std::setw(2) << hour
  		  << ":" << std::setw(2) << min
  		  << ":" << std::setw(2) << sec
  		  << "]";
  	continue;
      }
      std::stringstream stream;
      stream << tmp;
      stream >> key >> count;
      if( key == "Pi-Beam" ){
  	KBeam = count;
  	KBeamG = (double)KBeam/1.e9;
  	std::cout << std::setfill(' ')
  		  << "  Run# "     << std::setw(4) << irun
  		  << ",  Pi-Beam: " << std::setw(9) << KBeam
  		  << std::endl;;
  	KSum += KBeamG;
  	TDatime Time(year, month, day, hour, min, sec);
  	time.push_back(Time.Convert() + TimeOffset);
  	PionSum.push_back(KSum);
  	break;
      }
    }
  }

  //==== Draw =========
  TCanvas *cg = new TCanvas("cg", "cg");
  TGraph *g_Pion = new TGraph(time.size(), &time[0], &PionSum[0]);
  double xmin = StartTime.Convert() + TimeOffset;
  // double xmax = EndTime.Convert() + 2*24*60*60 + TimeOffset;
  double xmax = EndTime.Convert()+ TimeOffset;
  gStyle->SetNdivisions(505);
  g_Pion->GetXaxis()->SetTimeDisplay(1);
  g_Pion->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{%H:%M:%S}");
  g_Pion->SetMarkerSize(0);
  g_Pion->SetLineWidth(3.0);
  g_Pion->SetTitle("; ;Integrated Pion [G]");
  g_Pion->SetLineColor(kGreen+1);
  g_Pion->GetXaxis()->SetLimits(xmin, xmax);
  g_Pion->SetMinimum(0);
  g_Pion->SetMaximum(goalPi*1.1);
  g_Pion->Draw("AL");

  double Ksumpc = (double)KSum/goalPi*100.;
  TLatex *label = new TLatex();
  label->SetTextSize(0.05);
  label->DrawLatexNDC(0.75, 0.82, Form("%dG", goalPi));
  label->SetTextColor(kGreen+1);
  label->DrawLatexNDC(0.65, 0.24, Form("#splitline{%.1f G}{(%.1f %)}", KSum, Ksumpc));

  TF1 *fgoal = new TF1("fgoal", Form("%d", goalPi));
  fgoal->SetRange(xmin, xmax);
  fgoal->SetLineStyle(2);
  fgoal->SetLineColor(kOrange+7);
  fgoal->SetLineWidth(3.0);
  fgoal->Draw("same");

  double deadline = EndTime.Convert() + TimeOffset;
  TLine *lend = new TLine(deadline, 0, deadline, 300);
  lend->SetLineStyle(2);
  lend->Draw("same");

  
  return 0;

}
