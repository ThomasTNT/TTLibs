#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <string>
#include <vector>

#include "FileBrowser.h"
#include "TTImage.h"
#include "TTCallback.h"




namespace imgproc{

//******************************************************************************
//*                      (abstract)  ImageProcessor                            *
//*                                                                            *
//*                class for handling images and image sequences               *
//*                                                                            *                                             *                     
//******************************************************************************

enum ImageProcessState{ IMG_PROC_STATE_NOT_PROCESSED, IMG_PROC_STATE_IN_PROCESSING, IMG_PROC_STATE_PROCESSED };

//struct for holding the image data
class ImageSequenceEntry{
  public:
    ImageSequenceEntry();
    ~ImageSequenceEntry();

    void LoadImage();
    void UnloadImage(); 
    bool IsLoaded() const;
    ImageProcessState GetProcState() const;
    void SetProcState(const ImageProcessState state);

    image::TTFileImage * ImageFile = nullptr;
    image::TTMemoryImage * MemImage = nullptr;
    std::string Filename; 
    std::string Dirname;
    std::string Fullname;

    ImageProcessState ProcState;

};

//container for storing and handling the images
class ImageSequence{

  public:

    //constructor
    ImageSequence();

    //destructor
    ~ImageSequence();

    //set maximal memory usage
    //may be we made the memory management different in ther future
    void SetMaxMegaBytes(const unsigned int mb);

    //sets the infos for images but does not actually load the images
    void SetImageInfos(const std::string& directory, const std::vector<std::string>& filenames, const unsigned int before, const unsigned int after);

    //reads image infos by reading the first image
    void ReadImageInfos();

    //actually loads the images
    void LoadImages(const unsigned int pos, const int offset);

    //sets the current position to ther given position
    //reloads images im necessary
    void SetSequencePos(const unsigned int pos);

    //returns the current frame number
    unsigned int GetSequencePos() const;

    //return true if it is an image sequence
    bool IsSequence();
    
    //returns the ImageSequenceEntry for showing 
    const ImageSequenceEntry * GetImage(const unsigned int frameNumber) const;

    //loaded?
    bool IsLoaded(const unsigned int pos);

    //loaded?
    bool IsLoadedCompletey();

    //unload all images
    void UnloadAllImages();

  private:

    //clean up
    void Clear();

    //max memory 
    unsigned int MaxMegaBytes = 1024;

    //approx. size of one image in MB
    unsigned int ImagesToLoadAtOnce = 0; 

    //list of current images
    std::vector<ImageSequenceEntry*> Images;

    //currently loaded images 
    unsigned int CurrentlyLoadedImages = 0;

    //image infos
    unsigned int Width = 0;
    unsigned int Height = 0;
    unsigned int Channels = 0;
    unsigned int AlphaChannels = 0;
    unsigned int BytesPerChannel = 0;

    // image name data
    bool IsSeq = false;

    unsigned int FrameNumber = 0;

};

//empty abstract process parameters
class ImageProcessParameters{

};


//NOTE: a lot methods simply forward to inner ImageSequence
class ImageProcessor{

  public:

    //constructor
    ImageProcessor();

    //destructor
    virtual ~ImageProcessor();

    //sets image dimensions
    virtual void SetWindowDimensions(const unsigned int width, const unsigned int height);

    //getter
    virtual unsigned int GetWidth() const;

    //getter 
    virtual unsigned int GetHeight() const;

    //set maximal memory usage
    //may be we made the memory management different in ther future
    void SetMaxMegaBytes(const unsigned int mb);

    //opens image or image sequence
    void OpenImages(const std::string& directory, const std::vector<std::string>& filenames, const unsigned int before, const unsigned int after);

    //loaded?
    bool IsLoaded(const unsigned int pos);

    //loaded?
    bool IsLoadedCompletey();

    //sets the current position to ther given position
    //reloads images im necessary
    void SetSequencePos(const unsigned int pos);

    //returns the current frame number
    unsigned int GetSequencePos();

    //return true if it is an image sequence
    bool IsSequence();

    //returns the image for showing 
    virtual const image::TTImage * GetImageOriginal(const unsigned int frameNumber);

    //returns the image for showing 
    virtual const image::TTImage * GetImageProcessed(const unsigned int frameNumber);

    //returns the image for showing 
    virtual const image::TTImage * GetImageToShow(const unsigned int frameNumber);

    // processes current image
    virtual void ProcessImage(const imgproc::ImageProcessParameters& imageProcParams);

    //sets call back object for progress bar etc.
    void SetCallback(ttwin::TTCallback* const callback);

    //gui 
    virtual bool ReadyForAreaSelection(){ return false; };

    //gui select this part of the image
    virtual void AreaSelection(const unsigned int x0, const unsigned int y0, const unsigned int x1, const unsigned int y1){};

    //gui
    virtual std::vector<image::TTColorArea> GetAreasToDraw(){ return std::vector<image::TTColorArea>(); };

    //return the name of the currently loaded image
    //or "" if none is loaded
    std::string GetImageFilename() const;

  protected:

    //clear all
    virtual void Clear();

    //processes given image
    virtual void ProcessOneImage(const image::TTImage* const imageIn, image::TTImage* const imageOut, const imgproc::ImageProcessParameters& imageProcParams) = 0;

    //throws exception if image is invalid
    virtual void CheckImageProperties(const image::TTImage& img){};

    //allocates the process buffer used for keyers
    void AllocateProcBuffer(const size_t size);

    //the container that stores the images
    ImageSequence ImageSeq;

    //buffer for keyers
    size_t ProcBuffSize = 0;
    char * ProcBuffer = nullptr;

    //call back object for progress bar etc.
    ttwin::TTCallback* Callback = nullptr;
    
    unsigned int WindowWidth = 0;
    unsigned int WindowHeight = 0;
};

} //end of namespace imgproc

#endif