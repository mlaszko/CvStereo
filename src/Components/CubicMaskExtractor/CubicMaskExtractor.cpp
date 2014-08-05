/*!
 * \file
 * \brief
 * \author Lukasz Zmuda
 */

#include <memory>
#include <string>

#include "CubicMaskExtractor.hpp"
#include "Common/Logger.hpp"
#include <Types/MatrixTranslator.hpp>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include "Property.hpp"


namespace Processors {
namespace CubicMaskExtractor {

CubicMaskExtractor::CubicMaskExtractor(const std::string & name) :
		Base::Component(name),
		prop_XAxisMin_treshold("ransacFilter.XAxisMin_treshold", -0.5),
		prop_XAxisMax_treshold("ransacFilter.XAxisMax_treshold", 2.0),
		prop_YAxisMin_treshold("ransacFilter.YAxisMin_treshold", -2.0),
		prop_YAxisMax_treshold("ransacFilter.YAxisMax_treshold", 1.0),
		prop_ZAxisMin_treshold("ransacFilter.ZAxisMin_treshold", 0.5),
		prop_ZAxisMax_treshold("ransacFilter.ZAxisMax_treshold", 2.0)
{
		prop_XAxisMin_treshold.addConstraint("-5.0");
		prop_XAxisMin_treshold.addConstraint("5.0");
		prop_XAxisMax_treshold.addConstraint("-5.0");
		prop_XAxisMax_treshold.addConstraint("5.0");
		prop_YAxisMin_treshold.addConstraint("-5.0");
		prop_YAxisMin_treshold.addConstraint("5.0");
		prop_YAxisMax_treshold.addConstraint("-5.0");
		prop_YAxisMax_treshold.addConstraint("5.0");
		prop_ZAxisMin_treshold.addConstraint("-5.0");
		prop_ZAxisMin_treshold.addConstraint("5.0");
		prop_ZAxisMax_treshold.addConstraint("-5.0");
		prop_ZAxisMax_treshold.addConstraint("5.0");
		registerProperty(prop_XAxisMin_treshold);
		registerProperty(prop_XAxisMax_treshold);
		registerProperty(prop_YAxisMin_treshold);
		registerProperty(prop_YAxisMax_treshold);
		registerProperty(prop_ZAxisMin_treshold);
		registerProperty(prop_ZAxisMax_treshold);
} 

CubicMaskExtractor::~CubicMaskExtractor() {
  
}

void CubicMaskExtractor::prepareInterface() {
  
	// Register data streams, events and event handlers HERE!
	registerStream("in_image_xyz", &in_image_xyz);
	registerStream("in_centerMassPoint", &in_centerMassPoint);
	registerStream("out_mask", &out_mask);
	
	// Register handlers
	h_filter.setup(boost::bind(&CubicMaskExtractor::filter, this));
	registerHandler("filter", &h_filter);
	addDependency("filter", &in_image_xyz);

}

bool CubicMaskExtractor::onInit() {

	return true;
}

bool CubicMaskExtractor::onFinish() {
	return true;
}

bool CubicMaskExtractor::onStop() {
	return true;
}

bool CubicMaskExtractor::onStart() {
	return true;
}

void CubicMaskExtractor::filter() {
  
  // create images
  cv::Mat depth_image = in_image_xyz.read();
  cv::Point3d centerMassPoint = in_centerMassPoint.read();
  cv::Size depth_size = depth_image.size();
  
  double centerX = (double) centerMassPoint.x;
  double centerY = (double) centerMassPoint.y;
  double centerZ = (double) centerMassPoint.z;
  
  double xL= centerMassPoint.x-prop_XAxisMin_treshold;
  double xH= centerMassPoint.x+prop_XAxisMax_treshold;
  
  double yL= centerMassPoint.y-prop_YAxisMin_treshold;
  double yH= centerMassPoint.y+prop_YAxisMax_treshold;
   
  double zL= centerMassPoint.z-prop_ZAxisMin_treshold;
  double zH= centerMassPoint.z+prop_ZAxisMax_treshold;
  
  cv::Mat tmp_img;
  tmp_img.create(depth_size, CV_8UC1);
 
	std::cout << "Limity\n"<<"xL "<< xL<<"\txH "<< xH<< "\tyL " <<yL<<"\tyH "<< yH <<"\tzL "<< zL <<"\tzH" <<zH<<"\n";
	
	// iteracja po obrazie glebi, podstawienie pod maske odpowiednich pikseli
        if (depth_image.isContinuous() && tmp_img.isContinuous()) {
         depth_size.width *= depth_size.height;
         depth_size.height = 1;
        }
        depth_size.width *= 3;
        for (int i = 0; i < depth_size.height; i++)
        {
            const float* depth_ptr = depth_image.ptr <float> (i);
            uchar* tmp_p = tmp_img.ptr <uchar> (i);
            int j, k = 0;
	    int val = 0;
            for (j = 0; j < depth_size.width; j += 3)
            {
		 // get x, y, z from depth_image
		 float x = depth_ptr[j];
                float y = depth_ptr[j + 1];
                float z= depth_ptr[j + 2];;

		if(x>xL && x<xH && y>yL && y<yH &&z>zL && z<zH)
		{
		    val = 255;
		}
		else
		{
		    val=0;
		}
		tmp_p[k] = val;
		k++;
            }
        }
        
        out_mask.write(tmp_img);

   
}



} //: namespace CubicMaskExtractor
} //: namespace Processors
