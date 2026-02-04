

#include "exo-vtk-include.h"
#include <vtkCommand.h>
#include<vtkProperty.h>
#include <vtkBoxWidget.h>
#include <vtkTransform.h>

class vtkMyCallback : public vtkCommand

 {

public:

  static vtkMyCallback *New()

    { return new vtkMyCallback; }

  virtual void Execute(vtkObject *caller, unsigned long, void*)

    {

      vtkTransform *t = vtkTransform::New();

      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);

      widget->GetTransform(t);

      widget->GetProp3D()->SetUserTransform(t);

      t->Delete();

    }

 };

int main(int, char *[])
{
    
    vtkSmartPointer<vtkConeSource> coneSource =
    vtkSmartPointer<vtkConeSource>::New();
    coneSource->SetRadius(0.5);
    coneSource->SetHeight(3.0);
    coneSource->SetResolution( 10 );
    coneSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(coneSource->GetOutputPort());

    // Create an actor
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper); 
    actor->SetPosition(0.0, 1.0, 0.0);
    actor->GetProperty()->SetColor(0.0, 0.0, 1.0);

    // Créer un deuxième cone source et mapper pour le deuxième acteur
    vtkSmartPointer<vtkConeSource> coneSource2 =
    vtkSmartPointer<vtkConeSource>::New();
    coneSource2->SetRadius(0.5);
    coneSource2->SetHeight(3.0);
    coneSource2->SetResolution( 10 );
    coneSource2->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper2 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(coneSource2->GetOutputPort());

    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper2);
    actor2->SetPosition(0.0, 0.0, 0.0);
    actor2->GetProperty()->SetColor(1.0, 0.5, 0.0); 
    
  // Setup renderer, render window, and interactor
    vtkSmartPointer<vtkRenderer> renderer =vtkSmartPointer<vtkRenderer>::New();
    
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->SetBackground(1,1,1);
    renderer->GetActiveCamera()->SetPosition(5,10,10);
    renderer->GetActiveCamera()->SetFocalPoint(0,1,0);
    
    
   vtkSmartPointer<vtkRenderWindow> renderWindow =vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renderWindow);

  
  vtkBoxWidget *boxWidget = vtkBoxWidget::New();

  boxWidget->SetInteractor(iren);

  boxWidget->SetPlaceFactor(1.25);

  boxWidget->SetProp3D(actor);
  

  boxWidget->PlaceWidget();

  vtkMyCallback *callback = vtkMyCallback::New();

  boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  boxWidget->On();


  vtkBoxWidget *boxWidget2 = vtkBoxWidget::New();

  boxWidget2->SetInteractor(iren);

  boxWidget2->SetPlaceFactor(1.25);

  boxWidget2->SetProp3D(actor2);

  boxWidget2->PlaceWidget();

  vtkMyCallback *callback2 = vtkMyCallback::New();

  boxWidget2->AddObserver(vtkCommand::InteractionEvent, callback2);

  boxWidget2->On();

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}


