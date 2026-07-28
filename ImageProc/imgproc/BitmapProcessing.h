#ifndef __BitmapProcessing_H_
#define __BitmapProcessing_H_

#include "ColorServices.h"  //it defines the datatypes 

//***************************************************************
// here are some standalone functions for image processin
//
// the functions do not allocate dynamic memory. Instead
// you have to privide result and temp buffers from outside.
// The reason for this is to give the caller full access 
// over memory handling. 
//
// Centering - a thinning/erosion algorithm to find center points
//
// 
//***************************************************************

namespace imgproc{

//*****************************************************************************
//*                                 Centering                                 *
//*                                                                           *
//*  this is a kind of erosion algorithm                                      *
//*  it finds the centers of areas, not a skelleton but single center points  * 
//*****************************************************************************
template <class T, class R> 
void Centering(T*   in,                 //the input buffer, it will be changed! (must be U8, U16 or U32)
               U32* temp,               //must be U32, for storing the process pointers
               R*   result,             //the result buffer, can be the same as in or temp
               const R resultvalue,     //the value that indicates a set pixel for result
               const unsigned int w,    // the width of the image
               const unsigned int h);   // the height of the image


// ----------------------------- implementations --------------------------------
template <class T, class R> 
void Centering(T*   in,                 //the input buffer, it will be changed! 
               U32* temp,               //must be U32, for storing the process pointers
               R*   result,             //the result buffer, can be the same as in or temp
               const R resultvalue,     //the value that indicates a set pixel for result
               const unsigned int w,    // the width of the image
               const unsigned int h)    // the height of the image

{

  //indicating constants
  const T PIXEL_SET           = 0x01;  //00000001
  const T PIXEL_ISBORDER      = 0x02;  //00000010
  const T PIXEL_REMAINING     = 0x04;  //00000100
  const T PIXEL_PROCESSED     = 0x80;  //10000000

  //loop until something had changed
  U32*         &procBuffer = temp;  
  std::vector<unsigned int> areaList; //storing all pixels that belongs to the current area

  U32  processPointer;
  U32  writePointer;
  bool areaHasNonBorderPixel;
  U32  index, indexN;
  int  xg, yg;
  U32  xc, yc; //used to calculate x, y from index
  bool hasChanged;
  T  currValue; 

  bool firstRun = true; //for other interpretation of inbuffer 

  do{
    hasChanged = false;

    //------------------- mark all border pixels -------------------
    //marks all pixel which are set and are borders with PIXEL_ISBORDER
    for (unsigned int y = 0; y < h; y++){
      for (unsigned int x = 0; x < w; x++){
        currValue =   firstRun 
                    ? (in[x + y * w] > 0 ? PIXEL_SET : 0) // other interpretation of inbuffer 
                    : in[x + y * w]; //normal run
        if (currValue & PIXEL_SET){
          //look if there is a neighbour that is outside the area
          
          //upper row
          if (((x < w - 1) && (   (y > 0     && !(in[(x + 1) + (y - 1) * w] & PIXEL_SET))
                               || (             !(in[(x + 1) +  y      * w] & PIXEL_SET))
                               || (y < h - 1 && !(in[(x + 1) + (y + 1) * w] & PIXEL_SET))))
              //mid row
              || (y > 0                      && !(in[x + (y - 1) * w] & PIXEL_SET))
              || (y < h - 1                  && !(in[x + (y + 1) * w] & PIXEL_SET))
              //lower row
              || ((x > 0) && (   (y > 0     && !(in[(x - 1) + (y - 1) * w] & PIXEL_SET))
                              || (             !(in[(x - 1) +  y      * w] & PIXEL_SET))
                              || (y < h - 1 && !(in[(x - 1) + (y + 1) * w] & PIXEL_SET))))
             )
          {
            in[x + y * w] = currValue | PIXEL_ISBORDER;
          }
        }
      }
    }

    firstRun = false; 

    //------------------------ handle areas ------------------------------------
    
    // remove all border pixels that are in an area which has at least one
    // non-border pixel (PIXEL_SET)
    for (unsigned int y = 0; y < h; y++){
      for (unsigned int x = 0; x < w; x++){
      
        currValue = in[x + y * w];
      
        //loop until a nonempty and nonprocessed pixel was found
        if (currValue & PIXEL_SET && !(currValue & PIXEL_PROCESSED)){
          
          //this is a new area
          areaList.clear();
          processPointer = 0;   //current point to process
          writePointer   = 1;   //current postion where to add new points
          areaHasNonBorderPixel = false;
          
          //add current point a current processpoint
          index = x + y * w;
          procBuffer[processPointer] = index;
          
          //loop until processPointer == writePointer
          //in this case all pixels are processed and no pixel is left
          //-> area is complete
          while (processPointer < writePointer){
            
            //work on current processPointer
            index = procBuffer[processPointer];
            xc = index % w;
            yc = index / w;
            
            //search neighbours
            for (int i = 0; i < 3; i++){
              yg = static_cast<int>(yc) + i - 1;
              if (yg >= 0 && yg < static_cast<int>(h)){
                for (int j = 0; j < 3; j++){
                  if (i != j){
                    xg = static_cast<int>(xc) + j - 1;  
                    if (xg >= 0 && xg < static_cast<int>(w)){
                      currValue = in[xg + yg * w];
                      //is neighbour a pixel and not processed yet?
                      if ((currValue & PIXEL_SET) && !(currValue & PIXEL_PROCESSED)){
                        //then add it to the list of pixels to process
                        indexN = xg + yg * w;
                        procBuffer[writePointer] = indexN;
                        in[indexN] = in[indexN] | PIXEL_PROCESSED;
                        
                        ++writePointer;
                        //check if pixel is a non border pixel
                        if (!(currValue & PIXEL_ISBORDER)) areaHasNonBorderPixel = true;
                      }
                    }
                  }  
                }
              }
            }
            
            //when we are here than the current pixel was processed
            //so we add it to the current area and mark it
            areaList.push_back(index);
            
            //process next pixel
            ++processPointer;
          }
          
          //when we are here then we have the whole area
          if (areaHasNonBorderPixel){
            //in this case all pixel will be unset, but keep mark processed
            const unsigned int size = static_cast<unsigned int>(areaList.size());
            for (unsigned int i = 0; i < size; i++){
              index = areaList[i];
              currValue = in[index];
              if (currValue & PIXEL_ISBORDER){
                in[index] = currValue & ~PIXEL_SET;
                hasChanged = true;
              }
            }
          }
          else{
            
            //if an area has no nonboarder pixel anymore all pixel are border
            //pixels. All we have to do is to calculate the barycenter and choose
            //the nearest point
            
            //get barycenter
            unsigned int xsum = 0;
            unsigned int ysum  = 0;
            const unsigned int size = static_cast<unsigned int>(areaList.size());
            for (unsigned int i = 0; i < size; i++){
              index = areaList[i];   
              xsum += (index % w);
              ysum += (index / w);
            }
            const double xbc = static_cast<double>(xsum) / static_cast<double>(size);
            const double ybc = static_cast<double>(ysum) / static_cast<double>(size);
              
            //get nearest point
            unsigned int indexMin = 0;
            double minDist = 99999999;
            double dx, dy, d;
            for (unsigned int i = 0; i < size; i++){
              index = areaList[i];   
              dx = static_cast<double>(index % w) - xbc;
              dy = static_cast<double>(index / w) - ybc;
              d = sqrt(dx * dx + dy * dy);
              if (d < minDist){
                minDist = d;
                indexMin = i;  
              }
            }
            
            //remove all other points and mark nearest point as result
            for (unsigned int i = 0; i < size; i++){
              index = areaList[i];
              in[index] = in[index] & ~PIXEL_SET;
              if (i == indexMin){
                in[index] = in[index] | PIXEL_REMAINING;
              }
            }
          }
        }
      }
    }

    //-------- prepare the next run ------------
    for (unsigned int y = 0; y < h; y++){
      for (unsigned int x = 0; x < w; x++){
        currValue = in[x + y * w];
        //clean up all other flags except PIXEL_SET
        if (currValue & PIXEL_SET){
          in[x + y * w] = PIXEL_SET;
        }
      }
    }

  } while (hasChanged);

  //-------- set result ------------
  for (unsigned int y = 0; y < h; y++){
    for (unsigned int x = 0; x < w; x++){
      result[x + y * w] = (in[x + y * w] & PIXEL_REMAINING) ? resultvalue : 0;
    }
  }

}


} // end of namespace imgproc

#endif
