#include "ImageProcessor.h"



//******************************************************************************
//*                              ImageProcessor                                *
//*                                                                            *
//*                class for handling images and image sequences               *
//*                                                                            *                                             *                     
//*                                                                            *
//* created new 21.11.2014 by TNT                                              *
//******************************************************************************

//************
// constructor
//************
imgproc::ImageSequenceEntry::ImageSequenceEntry(){

}

//************
// destructor
//************
imgproc::ImageSequenceEntry::~ImageSequenceEntry(){
  UnloadImage();
}

//**********
// LoadImage
//**********
void imgproc::ImageSequenceEntry::LoadImage(){
  UnloadImage(); 

  ImageFile = image::GetTTFileImageByEnding(Filename);
  ImageFile->Open(Fullname);

  MemImage = new image::TTSimpleMemoryImage;
  MemImage->Create(ImageFile->Width(), ImageFile->Height(), image::RGBA, image::PROCTYPE);

}

//***********
//UnloadImage
//***********
void imgproc::ImageSequenceEntry::UnloadImage(){
  if (ImageFile){
    ImageFile->Close();
    delete ImageFile;
    ImageFile = nullptr;
  }
  if (MemImage){
    delete MemImage;
    MemImage = nullptr;
  }
}

//***********
//IsLoaded
//***********
bool imgproc::ImageSequenceEntry::IsLoaded() const{
  return ImageFile != nullptr;
}

//************
//GetProcState
//************
imgproc::ImageProcessState imgproc::ImageSequenceEntry::GetProcState() const{
  return ProcState;
}


//************
//SetProcState
//************
void imgproc::ImageSequenceEntry::SetProcState(const ImageProcessState state){
  ProcState = state;
}

//*******************************************************************************************************************

//************
// constructor
//************
imgproc::ImageSequence::ImageSequence(){

}

//************
// destructor
//************
imgproc::ImageSequence::~ImageSequence(){
  Clear();
}

//************
// destructor
//************
void imgproc::ImageSequence::Clear(){
  for (ImageSequenceEntry* entry : Images){
    delete entry;
  }
  Images.clear();
  CurrentlyLoadedImages = 0;
}

//********
//IsLoaded
//********
bool imgproc::ImageSequence::IsLoaded(const unsigned int pos){
  const ImageSequenceEntry * const img = !Images.empty() ? Images[pos] : nullptr;
  return img && img->IsLoaded();
}

//********
//IsLoadedCompletey
//********
bool imgproc::ImageSequence::IsLoadedCompletey(){
  if (!IsSeq){
    return IsLoaded(0);
  }
  else{
    return false; //TODO
  }
}

//*****************
// UnloadAllImages
//
//unload all images
//******************
void imgproc::ImageSequence::UnloadAllImages(){
  Clear();
}


//****************************************************************
// OpenImages
//
// sets the infos for images but does not actually load the images
//****************************************************************
void imgproc::ImageSequence::SetImageInfos(const std::string& directory, const std::vector<std::string>& filenames, const unsigned int before, const unsigned int after){
  //clear all old
  Clear();

  if (filenames.size() == 0){
    throw image::TTImageException("No filenames in list!");
  }
  //not a sequence
  else if (filenames.size() == 1){
    IsSeq = false;
    ImageSequenceEntry * const entry = new ImageSequenceEntry();
    entry->Dirname = directory;
    entry->Filename = filenames[0];
    entry->Fullname = ttutil::FileBrowser::join(entry->Dirname, entry->Filename);
    Images.push_back(entry);
  }
  else{
    IsSeq = true;
    //filenames are given in alphabetical order
    //we reorder them accourding to its number, and it must be a number, when its a sequence
    std::map<unsigned int, std::string> numberToFilename;
    std::vector<unsigned int> numbers;
    const std::string prefix = filenames[0].substr(0, before);
    const std::string suffix = filenames[0].substr(after);
    for (std::string filename : filenames){
      const std::string numberStr = filename.substr(before, after);
      const unsigned int number = ttutil::StringUtil::string2int(numberStr);
      if (numberToFilename.find(number) != numberToFilename.end()){
        throw image::TTImageException("Number " + ttutil::StringUtil::int2string(number) + " is already existing!");
      }
      numberToFilename[number] = filename;
      numbers.push_back(number);
    }
    
    std::sort(numbers.begin(), numbers.end());

    unsigned int preNumber = 0xffffffff;
    for (unsigned int number : numbers){
      //check if they are continously
      if (preNumber != 0xffffffff && number != (preNumber + 1)){
        std::cout << "Nicht fortlaufend!" << std::endl;
      }
      preNumber = number;

      ImageSequenceEntry * const entry = new ImageSequenceEntry();
      entry->Dirname = directory;
      entry->Filename = numberToFilename[number];
      entry->Fullname = ttutil::FileBrowser::join(entry->Dirname, entry->Filename);
      Images.push_back(entry);
    }
  }
}

//*********************************************
// ReadImageInfos
//
//*********************************************
// reads image infos by reading the first image
void imgproc::ImageSequence::ReadImageInfos(){
  //at first we guess the size we need for one image
  const std::string filename = Images[0]->Fullname;
  image::TTFileImage * const firstImage = image::GetTTFileImageByEnding(filename);
  firstImage->OpenFileInfo(filename);
  Width = firstImage->Width();
  Height = firstImage->Height();
  Channels = firstImage->ColorChannels();
  AlphaChannels = firstImage->AlphaChannels();
  BytesPerChannel = image::GetBytesByDatatyp(firstImage->Datatype());
  const unsigned int imageSizeMemImage = Width * Height * (Channels + AlphaChannels) * sizeof(image::t_proc);
  const unsigned int mbfactor = 1024 * 1024;
  const unsigned int memSizeMB = (imageSizeMemImage / mbfactor + (imageSizeMemImage % mbfactor > 0 ? 1 : 0));
  const unsigned int usedMegaBytes = firstImage->GetMemoryUsage() + memSizeMB;

  ImagesToLoadAtOnce = MaxMegaBytes / usedMegaBytes;
  if (ImagesToLoadAtOnce < 1) ImagesToLoadAtOnce = 1;
}


//**************************
// LoadImages
//
// actually loads the images
//**************************
void imgproc::ImageSequence::LoadImages(const unsigned int pos, const int offset){

  const unsigned int numberOfImages = Images.size();
  const unsigned int startPos =   (offset < 0) 
                                ? (static_cast<int>(pos) > abs(offset) ? static_cast<int>(pos)+offset : 0)
                                : (static_cast<int>(pos)+offset < static_cast<int>(numberOfImages - 1) ? static_cast<int>(pos) + offset : numberOfImages - 1);
  unsigned int imagesToLoad = numberOfImages - startPos;
  if (imagesToLoad > ImagesToLoadAtOnce) imagesToLoad = ImagesToLoadAtOnce;

  //check how many image we have to load
  //may be there are some already loaded
  unsigned int actuallyToLoad = 0;
  for (unsigned int p = startPos; p < (imagesToLoad + startPos); ++p){
    const ImageSequenceEntry * const img = Images[p];
    if (!img->IsLoaded()) ++actuallyToLoad;
  }

  //unload supernumeraries
  const unsigned int free = ImagesToLoadAtOnce - CurrentlyLoadedImages;
  const unsigned int toUnLoad = actuallyToLoad > free ? actuallyToLoad - free : 0;

  //unload before first, maybe we add a more 
  const unsigned int unloadBefore = toUnLoad > startPos ? startPos : toUnLoad;
  const unsigned int unloadAfter = toUnLoad - unloadBefore;

  unsigned int stilltoUnload = toUnLoad;
  for (unsigned int p = 0; p < startPos && stilltoUnload > 0; ++p){
    ImageSequenceEntry * const img = Images[p];
    if (img->IsLoaded()){
      img->UnloadImage();
      --stilltoUnload;
      --CurrentlyLoadedImages;
    }
  }
  for (unsigned int p = numberOfImages - 1; p > (imagesToLoad + startPos) && stilltoUnload > 0; ++p){
    ImageSequenceEntry * const img = Images[p];
    if (img->IsLoaded()){
      img->UnloadImage();
      --stilltoUnload;
      --CurrentlyLoadedImages;
    }
  }

  //actuall load the rest
  for (unsigned int p = startPos; p < (imagesToLoad + startPos); ++p){
    ImageSequenceEntry * const img = Images[p];
    if (!img->IsLoaded()){
      img->LoadImage();
      ++CurrentlyLoadedImages;
    }
  }

}



//*************************************************************
// SetMaxMegaBytes
//
//set maximal memory usage
//may be we made the memory management different in ther future
//*************************************************************
void imgproc::ImageSequence::SetMaxMegaBytes(const unsigned int mb){
  MaxMegaBytes = mb;
}

//**************************************************
// SetSequencePos
//
// sets the current position to ther given position
// reloads images im necessary
//**************************************************
void imgproc::ImageSequence::SetSequencePos(const unsigned int pos){
  if (IsSeq) {
    FrameNumber = pos;
    LoadImages(FrameNumber, 0); //to do some checks? 
  }
  else{
    if (pos > 0){
      throw image::TTImageException("There is only one image! You cannot set it to frame number " + ttutil::StringUtil::int2string(pos) + "!");
    }
  }

}

//*********************************
// GetSequencePos
// 
// returns the current frame number
//*********************************
unsigned int imgproc::ImageSequence::GetSequencePos() const {
  return IsSeq ? FrameNumber : 0;
}

//***************************************
// IsSequence
//
// return true if it is an image sequence
//***************************************
bool imgproc::ImageSequence::IsSequence(){
  return IsSeq;
}

//*******************************************
// GetImage
//
// returns the ImageSequenceEntry for showing 
//*******************************************
const imgproc::ImageSequenceEntry * imgproc::ImageSequence::GetImage(const unsigned int frameNumber) const{
  if (frameNumber >= Images.size()){
    throw image::TTImageException(ttutil::StringUtil::int2string(frameNumber) + " is an invalid frame number (" + ttutil::StringUtil::int2string(Images.size()) + ") !");
  }
  return Images[frameNumber];
}


//sets call back object for progress bar etc.
void SetCallback(const ttwin::TTCallback* Callback);


//*******************************************************************************************************************



//************
// constructor
//************
imgproc::ImageProcessor::ImageProcessor(){
  ImageSeq.SetMaxMegaBytes(1024);
}

//************
// destructor
//************
imgproc::ImageProcessor::~ImageProcessor(){
  delete[] ProcBuffer;
}

//**********************
// SetImageDimensions
//
// sets image dimensions
//**********************
void imgproc::ImageProcessor::SetWindowDimensions(const unsigned int width, const unsigned int height){
  WindowWidth = width;
  WindowHeight = height;
}

//*********
// GetWidth
//*********
unsigned int imgproc::ImageProcessor::GetWidth() const{
  const imgproc::ImageSequenceEntry * const imgEntry = ImageSeq.GetImage(0);
  return (imgEntry) ? imgEntry->ImageFile->Width() : 0;
}

//**********
// GetHeight
//**********
unsigned int imgproc::ImageProcessor::GetHeight() const{
  const imgproc::ImageSequenceEntry * const imgEntry = ImageSeq.GetImage(0);
  return (imgEntry) ? imgEntry->ImageFile->Height() : 0;
}

//*********************************************
// AllocateProcBuffer
// 
// allocates the process buffer used for keyers
//*********************************************
void imgproc::ImageProcessor::AllocateProcBuffer(const size_t size){
  if (!ProcBuffer || size > ProcBuffSize){
    delete[] ProcBuffer;
    ProcBuffSize = size;
    ProcBuffer = new char[ProcBuffSize];
  }
}


//*************************************************************
// SetMaxMegaBytes
//
//set maximal memory usage
//may be we made the memory management different in ther future
//*************************************************************
void imgproc::ImageProcessor::SetMaxMegaBytes(const unsigned int mb){
  ImageSeq.SetMaxMegaBytes(mb);
}


//**********
// Clear
//
// clear all
//**********
void imgproc::ImageProcessor::Clear(){
  ImageSeq.UnloadAllImages();
}

//*******************************
// OpenImages
//
// opens image or image sequence
//*******************************
void imgproc::ImageProcessor::OpenImages(const std::string& directory, const std::vector<std::string>& filenames, const unsigned int before, const unsigned int after){
  //unload all before
  Clear();
  try{
    ImageSeq.SetImageInfos(directory, filenames, before, after);
    ImageSeq.ReadImageInfos();
    ImageSeq.LoadImages(0, 0);
  }
  catch (image::TTImageException& ex){
    Clear();
    throw ex;
  }
  catch (...){
    Clear();
    throw;
  }
}

//********
//IsLoaded
//********
bool imgproc::ImageProcessor::IsLoaded(const unsigned int pos){
  return ImageSeq.IsLoaded(pos);
}

//********
//IsLoadedCompletey
//********
bool imgproc::ImageProcessor::IsLoadedCompletey(){
  return ImageSeq.IsLoadedCompletey();
}

//*******************************
// SetSequencePos
//
// sets the current position to ther given position
// reloads images im necessary
void imgproc::ImageProcessor::SetSequencePos(const unsigned int pos){
  ImageSeq.SetSequencePos(pos);
}

//*********************************
// GetSequencePos
// 
// returns the current frame number
//*********************************
unsigned int imgproc::ImageProcessor::GetSequencePos(){
  return ImageSeq.GetSequencePos();
}

//***************************************
// IsSequence
//
// return true if it is an image sequence
//***************************************
bool imgproc::ImageProcessor::IsSequence(){
  return ImageSeq.IsSequence();
}

//******************************
// GetImageShow
//
// returns the image for showing 
//******************************
const image::TTImage * imgproc::ImageProcessor::GetImageOriginal(const unsigned int frameNumber){
  const imgproc::ImageSequenceEntry * const imgEntry = ImageSeq.IsLoaded(frameNumber) ? ImageSeq.GetImage(frameNumber) : nullptr;
  if (!imgEntry) return nullptr;
  if (!imgEntry->IsLoaded()) return nullptr;

  //at first the original image
  return imgEntry->ImageFile;
}

//******************************
// GetImageShow
//
// returns the image for showing 
//******************************
const image::TTImage * imgproc::ImageProcessor::GetImageProcessed(const unsigned int frameNumber){
  const imgproc::ImageSequenceEntry * const imgEntry = ImageSeq.IsLoaded(frameNumber) ? ImageSeq.GetImage(frameNumber) : nullptr;
  if (!imgEntry) return nullptr;
  if (!imgEntry->IsLoaded()) return nullptr;

  //at first the original image
  return imgEntry->MemImage;
}

//*************************************************
// GetImageToShow
//
// returns the image for showing 
// used for image processors that know what to show
//*************************************************
const image::TTImage * imgproc::ImageProcessor::GetImageToShow(const unsigned int frameNumber){
  //default implementation
  return GetImageOriginal(frameNumber); 
}



//********************************************
// SetCallback
// 
// sets call back object for progress bar etc.
//********************************************
void imgproc::ImageProcessor::SetCallback(ttwin::TTCallback* const callback){
  Callback = callback;
}


//**********************************************
// GetImageFilename
//
// return the name of the currently loaded image
// or "" if none is loaded
//**********************************************
std::string imgproc::ImageProcessor::GetImageFilename() const{
  const unsigned int frameNumber = ImageSeq.GetSequencePos();
  const imgproc::ImageSequenceEntry * const imgEntry = ImageSeq.GetImage(frameNumber);
  if (!imgEntry) return "";
  if (!imgEntry->IsLoaded()) return "";
  return imgEntry->Filename;
}

//************************
// ProcessImage
//
// processes current image
//************************
void imgproc::ImageProcessor::ProcessImage(const imgproc::ImageProcessParameters& imageProcParams){
  const unsigned int frameNumber = ImageSeq.GetSequencePos();
  const imgproc::ImageSequenceEntry * const imgEntry = ImageSeq.GetImage(frameNumber);
  if (!imgEntry) return;
  if (!imgEntry->IsLoaded()) return;

  ProcessOneImage(imgEntry->ImageFile, imgEntry->MemImage, imageProcParams);

}

