

#include "exo-vtk-include.h"
#include "helpers.h"
#include "config.h"


int main(int, char *[])
{
    
    vtkDataSetReader *reader = vtkDataSetReader::New();
       reader->SetFileName((MY_DATA_PATH+ (std::string )"/noise.vtk").c_str());
       
       vtkDataSetMapper *mapper = vtkDataSetMapper::New();
       mapper->SetInputConnection(reader->GetOutputPort());
        mapper->SetScalarRange(1,6);
        vtkLookupTable *lut = vtkLookupTable::New();
        double vals[4] = { 0.75, 0, 0.05, 1 };
        lut->SetNumberOfColors(256);
    
        for (int i = 0 ; i < 256 ; i++)
          { 
              //ou selon les valeurs de i,  A prend les valeurs de 1 à 0 et B de 0 à 1
              double A = 1.0 - (double)i / 255.0; // A va de 1 à 0
              double B = (double)i / 255.0;       // B va de 0 à 1
              double vals[4] = { A, 0, B, 1 };
              lut->SetTableValue(i, vals);
          }

        mapper->SetLookupTable(lut);

        lut->Build();

      vtkContourFilter *cf = vtkContourFilter::New();
      cf->SetNumberOfContours(2);
      cf->SetValue(0,2.4);
      cf->SetValue(1,4.0);
      mapper->SetInputConnection(cf->GetOutputPort());
      cf->SetInputConnection(reader->GetOutputPort());
      
      // vtkDataSetMapper *mapper2 = vtkDataSetMapper::New();
      // vtkCutter *cutter = vtkCutter::New();
      // vtkPlane *plane = vtkPlane::New();        
      // cutter->SetCutFunction(plane);
      // plane->SetOrigin(0.0,0.0,0.0);
      // plane->SetNormal(0.0,1.0,1.0);
      // cutter->SetInputConnection(reader->GetOutputPort()); 
      // mapper2->SetInputConnection(cutter->GetOutputPort());
      // mapper2->SetScalarRange(1,6);
      // mapper2->SetLookupTable(lut);
      


      vtkActor *actor = vtkActor::New();
      actor->SetMapper(mapper);

      // vtkActor *actor2 = vtkActor::New();
      // actor2->SetMapper(mapper2);
    
      vtkRenderer *ren = vtkRenderer::New();
      ren->AddActor(actor);
      // ren->SetViewport(0.5, 0, 1, 1);

      // vtkRenderer *ren2 = vtkRenderer::New();
      // ren2->AddActor(actor2);
      // ren2->SetViewport(0, 0, 0.5, 1);
       
      vtkRenderWindow *renwin = vtkRenderWindow::New();
      renwin->SetSize(768, 768);
      renwin->AddRenderer(ren);
      // renwin->AddRenderer(ren2);

      int nbpas=2000;
      for (int i = 0 ; i < nbpas ; i++)
        {
            // Calculer la valeur entre 1 et 6
            double valeur = 1.0 + (5.0 * i) / (nbpas - 1);
            cf->SetValue(0, valeur); 
            renwin->Render();
        }
       
       

      //  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
      //  iren->SetRenderWindow(renwin);
      //  iren->Start();
  return EXIT_SUCCESS;
}


