#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include <TString.h>
#include <TGraph.h>
#include <TLine.h>
#include <TF1.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TDatime.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TROOT.h>

#define GIF 1

std::string getDatetimeStr();
Int_t getrun(std::string txtname);

#ifdef __CINT__
int IntegratedKaon(TString input)
#else
int main(int argc, char **argv)
#endif
{

#if GIF
  gROOT->SetBatch();
#endif

#ifdef __CINT__
  TString season = input;
#else
  TString season = argv[1];
#endif


  //==== setting =========
  TDatime StartTime(2025, 2, 10, 7, 0, 0);
  TDatime EndTime(2025, 3, 10, 23, 59, 0);
  int goalK   =   300;

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
  // const char* cmd = "ls -ltr ../scaler_2024may/ | tail -n 1 > tmp.txt";
  // const char* cmd = "ls -l ../scaler_2024may/ | tail -n 1 > tmp.txt";
  system(Form("ls -lt ../scaler_%s/scaler* | tail -n 1 > tmp_first.txt", season.Data()));
  system(Form("ls -l  ../scaler_%s/scaler* | tail -n 1 > tmp_last.txt",  season.Data()));
  Int_t firstrun = getrun("tmp_first.txt");
  Int_t lastrun  = getrun("tmp_last.txt");
  std::cout << "first run is Run#" << firstrun << std::endl;
  std::cout << "last  run is Run#" << lastrun  << std::endl;
  sleep(1);


  //===== Integrated Kaon ========
  std::vector<double> time;
  std::vector<double> KaonSum;

  double KSum = 0;
  for( int irun = firstrun; irun <= lastrun; irun++ ){
    // std::cout << "Run#" << irun << std::endl;
    std::string tmp;
    Bool_t flag = false;

    //==== get daq comment =================
    // std::string fname_comment = Form("/hdd1/e70_2024may/misc/comment.txt");
    std::string fname_comment = Form("/misc/raid/e70_%s/misc/comment.txt", season.Data());
    std::ifstream ifs_comment(fname_comment);
    if( !ifs_comment.is_open() ){
      std::cerr << " cannot open file : " << fname_comment << std::endl;
      return -1;
    }
    while( getline(ifs_comment, tmp) ){
      if( (tmp.find(Form("[RUN %5d] STOP", irun)) != std::string::npos) ){
	if( (tmp.find("AFT production") != std::string::npos) ||
	    (tmp.find("AFT KK production") != std::string::npos) ){
	  flag = true;
	  // std::cout << tmp << std::endl;
	  break;
	}
	else break;
      }
    }
    if( !flag ) continue;
    ifs_comment.close();

    //===== open file =======
    // std::string fname = Form("../scaler_2024may/scaler_%05d.txt", irun);
    std::string fname = Form("../scaler_%s/scaler_%05d.txt", season.Data(), irun);
    std::ifstream ifs(fname);
    if( !ifs.is_open() ){
      std::cerr << " cannot open file: " << fname << std::endl;
      continue;
    }

    //==== std::cout << count ========
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
	// std::cout << std::setfill('0') << std::right
	// 	  << "[" << std::setw(4) << year
	// 	  << "-" << std::setw(2) << month
	// 	  << "-" << std::setw(2) << day
	// 	  << " " << std::setw(2) << hour
	// 	  << ":" << std::setw(2) << min
	// 	  << ":" << std::setw(2) << sec
	// 	  << "]";
	continue;
      }
      std::stringstream stream;
      stream << tmp;
      stream >> key >> count;
      if( key == "K-Beam" ){
	KBeam = count;
	KBeamG = (double)KBeam/1.e9;
	// std::cout << std::setfill(' ')
	// 	  << "  Run# "     << std::setw(4) << irun
	// 	  << ",  K-Beam: " << std::setw(9) << KBeam
	// 	  << std::endl;;
	KSum += KBeamG;
	TDatime Time(year, month, day, hour, min, sec);
	time.push_back(Time.Convert() + TimeOffset);
	KaonSum.push_back(KSum);
	// std::cout << "Run#" << irun << ":  "
	// 	  << std::setw(10) << std::fixed << std::setprecision(6)
	// 	  << KBeamG << " GKaon (" << Time.AsString() << ")" << std::endl;
	break;
      }
    }
  }
  std::cout << "TOTAL: " << KSum << " GKaon" << std::endl;



  //==== Draw =========
  TCanvas *cg = new TCanvas("cg", "cg");
  TGraph *g_Kaon = new TGraph(time.size(), &time[0], &KaonSum[0]);
  double xmin = StartTime.Convert() + TimeOffset;
  double xmax = EndTime.Convert() + 1*24*60*60 + TimeOffset;
  // double xmax = EndTime.Convert()+ TimeOffset;
  // gStyle->SetNdivisions(510);
  g_Kaon->GetXaxis()->SetNdivisions(510);
  g_Kaon->GetXaxis()->SetTimeDisplay(1);
  g_Kaon->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{%H:%M:%S}");
  g_Kaon->SetMarkerSize(0);
  g_Kaon->SetLineWidth(3.0);
  g_Kaon->SetTitle("; ;Integrated Kaon [G]");
  g_Kaon->SetLineColor(kGreen+1);
  g_Kaon->GetXaxis()->SetLimits(xmin, xmax);
  g_Kaon->SetMinimum(0);
  g_Kaon->SetMaximum(goalK*1.1);
  g_Kaon->Draw("AL");

  double Ksumpc = (double)KSum/goalK*100.;
  double Xirate = 200.; // ~200. Xi per GKaon (E07->175, E05->305)
  TLatex *label = new TLatex();
  label->SetTextSize(0.05);
  label->DrawLatexNDC(0.75, 0.84, Form("%dG", goalK));
  label->SetTextColor(kGreen+1);
  label->DrawLatexNDC(0.65, 0.24, Form("#splitline{%.1f G (%.1f %)}{= %.0f #Xi ?}", KSum, Ksumpc, KSum*Xirate));


  label->SetTextSize(0.03);
  label->SetTextColor(kBlack);
  label->DrawLatexNDC(0.600, 0.950, Form("Last run: %5d", lastrun));
  TString date = "Last update: " + getDatetimeStr();
  std::cout << date << std::endl;
  label->DrawLatexNDC(0.600, 0.920, date);

  TF1 *fgoal = new TF1("fgoal", Form("%d", goalK));
  fgoal->SetRange(xmin, xmax);
  fgoal->SetLineStyle(2);
  fgoal->SetLineColor(kOrange+7);
  fgoal->SetLineWidth(3.0);
  fgoal->Draw("same");

  // double deadline = EndTime.Convert() + TimeOffset;
  // TLine *lend = new TLine(deadline, 0, deadline, goalK);
  // lend->SetLineStyle(2);
  // lend->Draw("same");

  cg->Print(Form("../pdf/IntegratedKaon_e70_%s.pdf", season.Data()));
  // cg->Print("hoge.pdf");

  return 0;

}

std::string getDatetimeStr()
{
  std::time_t t = time(nullptr);
  const tm* localTime = localtime(&t);
  std::stringstream s;
  s << localTime->tm_year + 1900 << "/";
  s << std::setw(2) << std::setfill('0') << localTime->tm_mon + 1 << "/";
  s << std::setw(2) << std::setfill('0') << localTime->tm_mday << " ";
  s << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":";
  s << std::setw(2) << std::setfill('0') << localTime->tm_min  << ":";
  s << std::setw(2) << std::setfill('0') << localTime->tm_sec;
  return s.str();
}

Int_t getrun(std::string txtname)
{
  std::ifstream ifs(txtname);
  if( !ifs.is_open() ){
    std::cerr << " cannot open file : " << txtname << std::endl;
    return -1;
  }
  TString line;
  if( ifs.good() ) line.ReadLine(ifs);
  Ssiz_t pos = line.Last('.');
  TString tmp = line(pos-5, pos);
  Int_t runno = tmp.Atoi();
  return runno;
}
