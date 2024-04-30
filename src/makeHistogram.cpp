#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <vector>
#include <stdlib.h>
#include <TFile.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <getopt.h>


void showHelp(const char *arg0){
    std::cout << arg0 << " {options}\n\n"
        << "Usage:\n"
        << "OPTIONS: -h, --help, -o, --xmin, --xmax, --nbins, --graph, --quiet\n"
        << "\t-h, --help: show this help\n"
        << "\t-o {filename}: write TH1 histogram into this file (in default, the histogram is saved in \"out.root\")\n"
        << "\t--xmin {min}: left edge of TH1F\n"
        << "\t--xmax {max}: right edge of TH1F\n"
        << "\t--nbins {nbin}: number of bins of TH1F\n"
        << "\t--graph: not make histogram but make 2D-graph\n"
        << "\t--grapherr: not make histogram but make 2D-graph with error bar\n"
        << "\t--quiet: does not show text-historgram\n"
        << "\n"
        << "Copyright 2021 Shota Izumiyama" << std::endl;
    return;
}

void drawHistgramCLI(const std::vector<double> &buf){
    auto minmax = std::minmax_element(buf.begin(), buf.end());
    const double ledge = *minmax.first - (*minmax.second - *minmax.first) * 0.1;
    const double redge = *minmax.second + (*minmax.second - *minmax.first) * 0.1;
    std::vector<double>hist(10,0.);
    if(redge > ledge){
        for(auto d = buf.begin(); d != buf.end(); d++){
            int b = 10 * (*d - ledge)/( redge - ledge);
            if(b >= 0 && b < 10)
                hist[b]+=1.;
        }
    }

    auto yminmax = std::minmax_element(hist.begin(), hist.end());
    const double ylow = *yminmax.first;
    const double yhigh = *yminmax.second;
    if( ylow < yhigh){
        for(auto b = hist.begin(); b != hist.end(); b++)
            *b = 10. * (double)(*b - ylow) /(double)( yhigh - ylow);
    }
    
    for(int i = 0; i < 10; i++){
        std::cout << " |";
        for(auto b = hist.begin(); b != hist.end(); b++)
            std::cout << (*b >=(10-i)?"*":" ");
        std::cout << std::endl;
    }
    std::cout << " +---------->" << std::endl;
    std::cout << "Axis:" << std::endl;
    std::cout << "[xmin:xmax] = [" << ledge << ":" << redge << "]" << std::endl;
    std::cout << "[ymin:ymax] = [" << ylow << ":" << yhigh << "]" << std::endl;

    return;
}



int main (int argc, char **argv){
    enum MODE {kHIST, kGRAPH, kGRAPHERR} mode = kHIST;

    int c;
    int digit_optind = 0;
    std::pair<double, double> hist_range { 0., 10e-9};
    int hist_nbins = 200;
    std::string ofname = "out.root";
    int texthist_on = 1;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"xmin",     required_argument, 0,  0 },
            {"xmax",     required_argument, 0,  0 },
            {"nbins",     required_argument, 0,  0 },
            {"help", no_argument, 0, 0},
            {"quiet", no_argument, 0, 0},
            {"graph", no_argument, 0, 0},
            {"grapherr", no_argument, 0, 0},
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "o:h",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                switch(option_index){
                    case 0:
                        hist_range.first = std::stod(optarg);
                        break;
                    case 1:
                        hist_range.second = std::stod(optarg);
                        break;
                    case 2:
                        hist_nbins = std::stoi(optarg);
                        break;
                    case 3:
                        showHelp(argv[0]);
                        return EXIT_SUCCESS;
                        break;
                    case 4:
                        texthist_on = 0;
                        break;
                    case 5:
                        mode = kGRAPH;
                        break;
                    case 6:
                        mode = kGRAPHERR;
                        break;
                    default:
                        break;
                }
                break;
            case 'o':
                ofname = std::string(optarg);
                break;
            case 'h':
                showHelp(argv[0]);
                return EXIT_SUCCESS;
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    //while (optind < argc)
    //    printf("%s ", argv[optind++]);


    TH1F *h = new TH1F("hist", ";;Entries / bin;", hist_nbins, hist_range.first, hist_range.second);
    h->Sumw2();
    TGraph *g;
    TGraphErrors *gE;

    std::vector<double> histdata;
    std::vector<double> graphX;
    std::vector<double> graphXE;
    std::vector<double> graphYE;

    for(std::string line; std::getline(std::cin, line);){
        double x, xe;
        double y, ye;
        size_t dpos;
        std::string xstr, ystr, xestr, yestr;
        try{
            switch(mode){
                case kHIST:
                    y = std::stod(line);
                    break;
                case kGRAPH:
                    dpos = line.find(' ');
                    xstr = line.substr(0, dpos);
                    ystr = line.substr(dpos+1);
                    x = std::stod(xstr);
                    y = std::stod(ystr);
                    break;
                case kGRAPHERR:
                    dpos = line.find(' ');
                    xstr = line.substr(0, dpos);
                    line = line.substr(dpos+1);
                    dpos = line.find(' ');
                    ystr = line.substr(0, dpos);
                    line = line.substr(dpos+1);
                    dpos = line.find(' ');
                    xestr = line.substr(0, dpos);
                    yestr = line.substr(dpos+1);
                    x = std::stod(xstr);
                    y = std::stod(ystr);
                    xe = std::stod(xestr);
                    ye = std::stod(yestr);
                    break;
            }
        } catch (const std::invalid_argument& e){
            continue;
        }
        h->Fill(y);
        histdata.push_back(y);
        graphX.push_back(x);
        if( mode == kGRAPHERR ){
          graphXE.push_back(xe);
          graphYE.push_back(ye);
        }
    }
    if(texthist_on > 0)
        drawHistgramCLI(histdata);

    if(histdata.size()>0){
        double mean = std::accumulate(histdata.begin(), histdata.end(), 0.) / (double)histdata.size();
        std::cout << "Mean: " <<  mean << std::endl;
        auto calc_square = [](double sum, double a){ return sum + TMath::Power(a, 2);};
        std::cout << "StdDev: " <<  TMath::Sqrt(std::accumulate(histdata.begin(), histdata.end(), 0., calc_square) / (double)histdata.size() - TMath::Power(mean,2))  << std::endl;
        //std::cout << "StdDev(ROOT): " <<  TMath::StdDev(histdata.begin(), histdata.end())  << std::endl;
    }
    TFile *f = new TFile(ofname.c_str(), "RECREATE");
    h->Write();
    if(mode==kGRAPH){
        g = new TGraph(graphX.size(), &graphX[0], &histdata[0]);
        g->Write("graph");
    } else if ( mode == kGRAPHERR ) {
        gE = new TGraphErrors(graphX.size(), &graphX[0], &histdata[0], &graphXE[0], &graphYE[0]);
        gE->Write("graph");
    }

    f->Save();
    f->Close();
    f->Delete();

    return EXIT_SUCCESS;
}
