

#include "exo-vtk-include.h"
#include "helpers.h"
#include "config.h"


//ICI PLACER LA TAILLE DU FICHIER 512 ou 1024
#define TAILLE 512
//#define TAILLE 1024



int gridSize = TAILLE;



int winSize = 768;

using std::cerr;
using std::endl;

// Function prototypes
vtkRectilinearGrid  *ReadGrid(int zStart, int zEnd);


int main(int argc, char *argv[])
{   GetMemorySize("initialization");
    int t1;
    t1 = timer->StartTimer();
    

         int zStart = 0;
         int zEnd = zStart+(gridSize)-1;
           
         GetMemorySize("before read");
           
         vtkRectilinearGrid *rg =  ReadGrid(zStart, zEnd);
           
         GetMemorySize("after  read");
              

         vtkDataSetMapper *mapper = vtkDataSetMapper::New();

         double normal[3] = { 0., 0., 1.0 };
         vtkCutter *cutter = vtkCutter::New();
         vtkPlane *plane = vtkPlane::New();

         cutter->SetCutFunction(plane);
         cutter->SetInputData(rg);
    
         mapper->SetInputConnection(cutter->GetOutputPort());
    


         vtkLookupTable *lut = vtkLookupTable::New();
         lut->Build();

         mapper->SetScalarRange(0,15);
         mapper->SetLookupTable(lut);

        
         vtkActor *actor = vtkActor::New();
         actor->SetMapper(mapper);

         unsigned int n = 20;
         double opacity = 1.0 / (static_cast<double>(n)) * 5.0;
         actor->GetProperty()->SetOpacity(opacity);
         cutter->GenerateValues(n, -0.5, 0.5);
         lut->SetAlphaRange(opacity, opacity);

         vtkRenderer *ren = vtkRenderer::New();
         ren->AddActor(actor);

         vtkCamera *cam = ren->GetActiveCamera();
         
         cam->ParallelProjectionOn();
         cam->SetFocalPoint(0.5, 0.5, 0.5);
         cam->SetPosition(0.5, 0.5, 2.5);
         ren->GetActiveCamera()->SetParallelScale(0.5);
    
         plane->SetNormal(ren->GetActiveCamera()->GetViewPlaneNormal());
         plane->SetOrigin(ren->GetActiveCamera()->GetFocalPoint());
         
         double origin[3] = { 0.5, 0.5,0.5};
         plane->SetOrigin(origin);
    
         vtkRenderWindow *renwin = vtkRenderWindow::New();
         
         renwin->SetSize(winSize, winSize);
         renwin->AddRenderer(ren);

 
         renwin->Render();
         
         
         vtkNew<vtkRenderWindowInteractor> iren;

         iren->SetRenderWindow(renwin);

        iren->Start();
         
         
         GetMemorySize("end");
         timer->StopTimer(t1,"time");
        
         
       
         
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

/*
    vtkFloatArray *pr = vtkFloatArray::New();
    pr->SetNumberOfTuples(valuesToRead);
    for (int i = 0 ; i < valuesToRead ; i++)
        pr->SetTuple1(i, parRank);

    pr->SetName("par_rank");
    rg->GetPointData()->AddArray(pr);
    pr->Delete();
 */

    rg->GetPointData()->SetActiveScalars("entropy");
    
    cerr << file << "  Done reading" << endl;
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
        
        rgba_out[i*4] = rgba_out[i*4+1] = rgba_out[i*4+2] =(zbuffer[i]==1.0?0:coef*(zbuffer[i]-min));
        
        rgba_out[i*4+3] = 0.0;
    }
    
    
    return false;
}


