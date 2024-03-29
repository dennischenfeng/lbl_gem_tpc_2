// Fitting of a TGraph2D with a 3D straight line, slightly edited by Dennis Feng for the DM-TPC project
//
// Author: L. Moneta
//


#include <TMath.h>
#include <TGraph2D.h>
#include <TRandom2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <Math/Functor.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <Fit/Fitter.h>

// using namespace ROOT::Math;


// define the parameteric line equation
void line(double t, const double *p, double &x, double &y, double &z) {
   // a parameteric line is define from 6 parameters but 4 are independent
   // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
   // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1;
   x = p[0] + p[1]*t;
   y = p[2] + p[3]*t;
   z = t;
}

// calculate distance line-point
double distance2(double x,double y,double z, const double *p) {
   // distance line point is D= | (xp-x0) cross  ux |
   // where ux is direction of line and x0 is a point in the line (like t = 0)
   ROOT::Math::XYZVector xp(x,y,z);
   ROOT::Math::XYZVector x0(p[0], p[2], 0. );
   ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], 1. );
   ROOT::Math::XYZVector u = (x1-x0).Unit();
   double d2 = ((xp-x0).Cross(u)) .Mag2();
   return d2;
}
bool first = true;


// function Object to be minimized
struct SumDistance2 {
   // the TGraph is a data member of the object
   TGraph2D * fGraph;

   SumDistance2(TGraph2D * g) : fGraph(g) {}

   // implementation of the function to be minimized
   double operator() (const double * par) {

      assert(fGraph    != 0);
      double * x = fGraph->GetX();
      double * y = fGraph->GetY();
      double * z = fGraph->GetZ();
      int npoints = fGraph->GetN();
      double sum = 0;
      for (int i  = 0; i < npoints; ++i) {
         double d = distance2(x[i],y[i],z[i],par);
         sum += d;
#ifdef DEBUG
         if (first) std::cout << "point " << i << "\t"
            << x[i] << "\t"
            << y[i] << "\t"
            << z[i] << "\t"
            << std::sqrt(d) << std::endl;
#endif
      }
      if (first)
         std::cout << "Total Initial distance square = " << sum << std::endl;
      first = false;
      return sum;
   }

};

Int_t line3dfit_copy()
{
   gStyle->SetOptStat(0);
   gStyle->SetOptFit();


   //double e = 0.1;
   // Int_t nd = 5;


//    double xmin = 0; double ymin = 0;
//    double xmax = 10; double ymax = 10;

   TGraph2D * gr = new TGraph2D();

   // Fill the 2D graph
   // double p0[4] = {10,20,1,2};

   // generate graph with the 3d points
   // for (Int_t N=0; N<nd; N++) {
   //    double x,y,z = 0;
   //    // Generate a random number
   //    double t = gRandom->Uniform(0,10);
   //    line(t,p0,x,y,z);
   //    double err = 1;
   //  // do a gaussian smearing around the points in all coordinates
   //    x += gRandom->Gaus(0,err);
   //    y += gRandom->Gaus(0,err);
   //    z += gRandom->Gaus(0,err);
   //    gr->SetPoint(N,x,y,z);
   //    //dt->SetPointError(N,0,0,err);
   // }


   // gr->SetPoint(0, 3, 3, 5);
   // gr->SetPoint(1, 3, 3, 3);
   // gr->SetPoint(2, 4, 3.5, 5);
   // gr->SetPoint(3, 6.2, 7.3, 5.8);
   // gr->SetPoint(4, 9, 8, 7);
   // gr->SetPoint(5, 5, 5, 5);

   gr->SetPoint(0, 19, -4.25, 1.92);
   gr->SetPoint(1, 19, -4.30, 1.92);
   gr->SetPoint(2, 19, -4.35, 1.92);
   gr->SetPoint(3, 19, -4.40, 1.92);
   gr->SetPoint(4, 19, -4.45, 1.92);
   gr->SetPoint(5, 19, -4.50, 1.92);
   gr->SetPoint(6, 19, -4.55, 1.92);
   gr->SetPoint(7, 19, -4.60, 1.92);
   gr->SetPoint(8, 18.75, -4.30, 1.92);
   gr->SetPoint(9, 18.75, -4.35, 1.92);
   gr->SetPoint(10, 18.75, -4.40, 1.92);
   gr->SetPoint(11, 18.75, -4.45, 1.92);
   gr->SetPoint(12, 18.75, -4.50, 1.92);
   gr->SetPoint(13, 18.75, -4.55, 1.92);
   gr->SetPoint(14, 19, -4.20, 2.08);
   gr->SetPoint(15, 19, -4.65, 2.08);
   gr->SetPoint(16, 19, -4.70, 2.08);
   gr->SetPoint(17, 18.75, -4.25, 2.08);
   gr->SetPoint(18, 18.75, -4.60, 2.08);
   // gr->SetPoint(19, 19.001, -4.151, 2.241);
   // gr->SetPoint(20, 18.751, -4.201, 2.241);
   gr->SetPoint(19, 19., -4.15, 2.24);
   gr->SetPoint(20, 18.5723, -4.20, 2.24);

   // gr->SetMarkerStyle(8);
   // gr->Draw("p");
   
   // gr->Draw("p0");
   // TFitResultPtr fit = gr->Fit("pol1", "WS");
   // // fit->Print("V");
   // Double_t p0 = fit->Value(0);
   // Double_t p1 = fit->Value(1);

   // // draw the line
   // TPolyLine3D *l = new TPolyLine3D(2);
   // double dz = 8;
   // l->SetPoint(0,0,0,p0);
   // l->SetPoint(1,dz,0,dz * p1);

   // l->SetLineColor(kRed);
   // l->Draw("same");

   // fit the graph now, and make the functor objet

   ROOT::Fit::Fitter  fitter;

   SumDistance2 sdist(gr);
#ifdef __CINT__
   ROOT::Math::Functor fcn(&sdist,4,"SumDistance2");
#else
   ROOT::Math::Functor fcn(sdist,4);
#endif
   // set the function and the initial parameter values
   double pStart[4] = {1,1,1,1};
   fitter.SetFCN(fcn,pStart);
   // set step sizes different than default ones (0.3 times parameter values)
   for (int i = 0; i < 4; ++i) fitter.Config().ParSettings(i).SetStepSize(0.01);

   bool ok = fitter.FitFCN();
   if (!ok) {
      Error("line3Dfit","Line3D Fit failed");
      return 1;
   }

   const ROOT::Fit::FitResult & result = fitter.Result();

   std::cout << "Total final distance square " << result.MinFcnValue() << std::endl;
   // result.Print(std::cout); // @@@ suppress output


   // get fit parameters
   const double * parFit = result.GetParams();

   // draw the fitted line
   int n = 1000;
   double t0 = 0;
   double dt = 10;
   TPolyLine3D *l = new TPolyLine3D(n);
   for (int i = 0; i <n;++i) {
      double t = t0+ dt*i/n;
      double x,y,z;
      line(t,parFit,x,y,z);
      l->SetPoint(i,x,y,z);
   }
   l->SetLineColor(kRed);
   l->Draw("same");

   // Access to fit params and minfcnvalue
   cout << parFit[1] << "\n";
   cout << result.MinFcnValue() << "\n";


   // // draw original line
   // TPolyLine3D *l0 = new TPolyLine3D(n);
   // for (int i = 0; i <n;++i) {
   //    double t = t0+ dt*i/n;
   //    double x,y,z;
   //    line(t,p0,x,y,z);
   //    l0->SetPoint(i,x,y,z);
   // }
   // l0->SetLineColor(kBlue);
   // l0->Draw("same");
   

   return 0;
}

// int main() {
//    return line3dfit_copy();
// }
