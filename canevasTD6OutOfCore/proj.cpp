

#include "exo-vtk-include.h"
#include "helpers.h"
#include "config.h"



//ICI PLACER LA TAILLE DU FICHIER 512 ou 1024
#define TAILLE 512
//#define TAILLE 1024



int gridSize = TAILLE;



int winSize = 768;

int NbPasses = 27; 
int passNum ;

using std::cerr;
using std::endl;

// Function prototypes
vtkRectilinearGrid  *ReadGrid(int zStart, int zEnd);
void                 WriteImage(const char *name, const float *rgba, int width, int height);
bool ComposeImageZbuffer(float *rgba_out, float *zbuffer,   int image_width, int image_height);


int main(int argc, char *argv[])
{    std::string p;

    GetMemorySize("initialization");
    int t1;
    t1 = timer->StartTimer();
    
    // Set up the pipeline.
    vtkContourFilter *cf = vtkContourFilter::New();
    cf->SetNumberOfContours(1);
    cf->SetValue(0, 5.0);
    
    vtkDataSetMapper *mapper = vtkDataSetMapper::New();
    mapper->SetInputConnection(cf->GetOutputPort());

    vtkLookupTable *lut = vtkLookupTable::New();
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(0, NbPasses);

    // pseudo-volume alpha
    unsigned int nPasses = NbPasses;
    double opacity = 0.9;
    lut->SetAlphaRange(1.0, 1.0);
    lut->Build();


    vtkActor *actor = vtkActor::New();
    actor->SetMapper(mapper);

    vtkRenderer *ren = vtkRenderer::New();
    ren->AddActor(actor);   
        
    vtkCamera *cam = ren->GetActiveCamera();
    cam->SetFocalPoint(0.5, 0.5, 0.5);
    cam->SetPosition(1.5, 1.5, 1.5);
    
    vtkRenderWindow *renwin = vtkRenderWindow::New();
    // THIS DOESN'T WORK WITHOUT MESA
    renwin->SetMultiSamples(1);
    renwin->OffScreenRenderingOn();
    renwin->SetSize(winSize, winSize);
    renwin->AddRenderer(ren);
    
    // Read the data.
    
    passNum=0;
    int zStart = 0;
    int zEnd = gridSize-1;
    
    float *rgba = new float[4*winSize*winSize];
    float *auxrgba    = new float[4*winSize*winSize];

    float *auxzbuffer = new float[winSize*winSize];

    // Initialiser : fond bleu, zbuffer à 1.0 (infini)
    for (int i = 0; i < winSize*winSize; i++) {
        auxzbuffer[i]  = 1.0f;
        auxrgba[i*4]   = 0.0f;  // R
        auxrgba[i*4+1] = 0.0f;  // G
        auxrgba[i*4+2] = 1.0f;  // B bleu
        auxrgba[i*4+3] = 0.0f;  // A
    }
     
     
     for( passNum=0; passNum<NbPasses;passNum++){
        
        int remainder = gridSize % NbPasses;
        int step = gridSize / NbPasses;
        int zStart = passNum * step + std::min(passNum, remainder);
        int zEnd = zStart + step - 1 + (passNum < remainder ? 1 : 0);
        if (passNum == NbPasses-1) zEnd = gridSize-1; 
                
         std::cout<<"passe "<<passNum<<": zStart: "<<zStart<<std::endl;
         std::cout<<"passe "<<passNum<<": zEnd: "<<zEnd<<std::endl;
         std::cout<<"passe "<<passNum<<": ecart: "<<zEnd-zStart<<std::endl;
       
         
         
       GetMemorySize(("Pass "+std::to_string(NbPasses)+ " before read").c_str());
    vtkRectilinearGrid *rg = ReadGrid(zStart, zEnd);
      GetMemorySize(("Pass "+std::to_string(passNum)+ " after  read").c_str());
    
    cf->SetInputData(rg);
    rg->Delete();
    
    // Force an update and set the parallel rank as the active scalars.
    cf->Update();
    cf->GetOutput()->GetPointData()->SetActiveScalars("pass_num");
    
    renwin->Render();
    
    float *pass_rgba = renwin->GetRGBAPixelData(0, 0, winSize-1, winSize-1, 1);
    float *zbuffer = renwin->GetZbufferData(0, 0, winSize-1, winSize-1);

    // Forcer alpha : 0.5 si pixel non vide, 0 si fond
    for (int i = 0; i < winSize*winSize; i++) {
        if (zbuffer[i] < auxzbuffer[i]) {   // pixel plus proche ?
            auxzbuffer[i]  = zbuffer[i];
            auxrgba[i*4]   = pass_rgba[i*4];
            auxrgba[i*4+1] = pass_rgba[i*4+1];
            auxrgba[i*4+2] = pass_rgba[i*4+2];
            auxrgba[i*4+3] = pass_rgba[i*4+3];
        }
    }

            
    std::string name_anim = "anim_" + std::to_string(passNum) + ".png";
    WriteImage(name_anim.c_str(), auxrgba, winSize, winSize);

    std::string name = "image" + std::to_string(passNum) + ".png";
    WriteImage(name.c_str(), pass_rgba, winSize, winSize);

    float *new_rgba = new float[4*winSize*winSize];
    bool didComposite = ComposeImageZbuffer(new_rgba, zbuffer, winSize, winSize);
    name = "imageZbuffer" + std::to_string(passNum) + ".png";
    WriteImage(name.c_str(), new_rgba, winSize, winSize);

    free(pass_rgba);
    free(zbuffer);
    free(new_rgba);
    }//fin du for 
    
    
    WriteImage("final_image.png", auxrgba, winSize, winSize);

    delete[] auxrgba;
    delete[] auxzbuffer;
    delete[] rgba;

    GetMemorySize("end");
    timer->StopTimer(t1,"time");
    
    
}


// You should not need to modify these routines.
vtkRectilinearGrid *
ReadGrid(int zStart, int zEnd)
{
    int  i;
    std::string file=(MY_DATA_PATH+ (std::string )"/sn_resamp"+std::to_string(TAILLE));
    if (zStart < 0 || zEnd < 0 || zStart >= gridSize || zEnd >= gridSize || zStart > zEnd)
    {
        cerr << file << "  Invalid range: " << zStart << "-" << zEnd << endl;
        return NULL;
    }

    ifstream ifile(file.c_str());
    if (ifile.fail())
    {
        cerr << file << "  Unable to open file: " << file << "!!" << endl;
    }

    cerr << file << "  Reading from " << zStart << " to " << zEnd << endl;
    vtkRectilinearGrid *rg = vtkRectilinearGrid::New();
    vtkFloatArray *X = vtkFloatArray::New();
    X->SetNumberOfTuples(gridSize);
    for (i = 0 ; i < gridSize ; i++)
        X->SetTuple1(i, i/(gridSize-1.0));
    rg->SetXCoordinates(X);
    X->Delete();
    vtkFloatArray *Y = vtkFloatArray::New();
    Y->SetNumberOfTuples(gridSize);
    for (i = 0 ; i < gridSize ; i++)
        Y->SetTuple1(i, i/(gridSize-1.0));
    rg->SetYCoordinates(Y);
    Y->Delete();
    vtkFloatArray *Z = vtkFloatArray::New();
    int numSlicesToRead = zEnd-zStart+1;
    Z->SetNumberOfTuples(numSlicesToRead);
    for (i = zStart ; i <= zEnd ; i++)
        Z->SetTuple1(i-zStart, i/(gridSize-1.0));
    rg->SetZCoordinates(Z);
    Z->Delete();
    
    rg->SetDimensions(gridSize, gridSize, numSlicesToRead);
    
    int valuesPerSlice  = gridSize*gridSize;
    int bytesPerSlice   = 4*valuesPerSlice;
  
#if TAILLE == 512
    unsigned int offset          = (unsigned int)zStart * (unsigned int)bytesPerSlice;
    unsigned int bytesToRead     = bytesPerSlice*numSlicesToRead;
    unsigned int valuesToRead    = valuesPerSlice*numSlicesToRead;
#elif TAILLE == 1024
    unsigned long long offset          = (unsigned long long)zStart * bytesPerSlice;
    unsigned long long  bytesToRead     = (unsigned long long )bytesPerSlice*numSlicesToRead;
    unsigned int valuesToRead    = (unsigned int )valuesPerSlice*numSlicesToRead;
#else
#error Unsupported choice setting
#endif

    vtkFloatArray *scalars = vtkFloatArray::New();
    scalars->SetNumberOfTuples(valuesToRead);
    float *arr = scalars->GetPointer(0);
    ifile.seekg(offset, ios::beg);
    ifile.read((char *)arr, bytesToRead);
    ifile.close();
    
    scalars->SetName("entropy");
    rg->GetPointData()->AddArray(scalars);
    scalars->Delete();
    
    vtkFloatArray *pr = vtkFloatArray::New();
    pr->SetNumberOfTuples(valuesToRead);
    for (int i = 0 ; i < valuesToRead ; i++)
        pr->SetTuple1(i, passNum);
    
    pr->SetName("pass_num");
    rg->GetPointData()->AddArray(pr);
    pr->Delete();
    
    rg->GetPointData()->SetActiveScalars("entropy");
    
    cerr << file << " Done reading" << endl;
    return rg;
}


void
WriteImage(const char *name, const float *rgba, int width, int height)
{
    vtkImageData *img = vtkImageData::New();
     img->SetDimensions(width, height, 1);
#if VTK_MAJOR_VERSION <= 5
     img->SetNumberOfScalarComponents(3);
    img->SetScalarTypeToUnsignedChar();
#else
    img->AllocateScalars(VTK_UNSIGNED_CHAR,3);
#endif
    
    for (int i = 0 ; i < width ; i++)
        for (int j = 0 ; j < height ; j++)
        {
            unsigned char *ptr = (unsigned char *) img->GetScalarPointer(i, j, 0);
            int idx = j*width + i;
            ptr[0] = (unsigned char) (255*rgba[4*idx + 0]);
            ptr[1] = (unsigned char) (255*rgba[4*idx + 1]);
            ptr[2] = (unsigned char) (255*rgba[4*idx + 2]);
        }
    
    
    vtkPNGWriter *writer = vtkPNGWriter::New();
    writer->SetInputData(img);
    writer->SetFileName(name);
    writer->Write();
    
    img->Delete();
    writer->Delete();
}

bool ComposeImageZbuffer(float *rgba_out, float *zbuffer,   int image_width, int image_height)
{
    int npixels = image_width*image_height;
    
    float min=1;
    float max=0;
    for (int i = 0 ; i < npixels ; i++){
        if ((zbuffer[i]<min) && (zbuffer[i] != 0)) min=zbuffer[i];
        if ((zbuffer[i]>max) && (zbuffer[i] != 1)) max=zbuffer[i];
        
    }
    std::cout<<"min:"<<min;
    std::cout<<"max:"<<max<<"  ";
    
    float coef=1.0/((max-min));
    
    
    std::cout<<"coef:"<<coef<<"  ";
    
    for (int i = 0 ; i < npixels ; i++){
        
        rgba_out[i*4] = rgba_out[i*4+1] = rgba_out[i*4+2] =(zbuffer[i]==1.0?0:(1-coef*(zbuffer[i]-min)));
        
        rgba_out[i*4+3] = 1.0;
    }
    
    
    return false;
}


