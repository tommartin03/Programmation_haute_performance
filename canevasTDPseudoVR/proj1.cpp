

#include "exo-vtk-include.h"

#include "config.h"


int main(int argc, char *argv[])
  {

    vtkNew<vtkNamedColors> colors;

    // Create pipeline. Read structured grid data.
    //
    vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
    pl3d->SetXYZFileName((MY_DATA_PATH+ (std::string )"/combxyz.bin").c_str());
    pl3d->SetQFileName((MY_DATA_PATH+ (std::string )"/combq.bin").c_str());
    pl3d->SetScalarFunctionNumber(100);
    pl3d->SetVectorFunctionNumber(202);
    pl3d->Update();

    vtkStructuredGrid* pl3dOutput =
        dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

    // A convenience, use this filter to limit data for experimentation.
    vtkNew<vtkExtractGrid> extract;
    extract->SetVOI(1, 55, -1000, 1000, -1000, 1000);
    extract->SetInputData(pl3dOutput);

    // The (implicit) plane is used to do the cutting
    vtkNew<vtkPlane> plane;
    plane->SetOrigin(0, 4, 2);
    plane->SetNormal(0, 1, 0);

    // The cutter is set up to process each contour value over all cells
    // (SetSortByToSortByCell). This results in an ordered output of polygons
    // which is key to the compositing.
    vtkNew<vtkCutter> cutter;
    cutter->SetInputConnection(extract->GetOutputPort());
    cutter->SetCutFunction(plane);
    cutter->GenerateCutScalarsOff();
 //   cutter->SetSortByToSortByCell();

    vtkNew<vtkLookupTable> clut;
    clut->SetHueRange(0, .67);
    clut->Build();

    vtkNew<vtkPolyDataMapper> cutterMapper;
    cutterMapper->SetInputConnection(cutter->GetOutputPort());
    cutterMapper->SetScalarRange(.18, .7);
    cutterMapper->SetLookupTable(clut);

    vtkNew<vtkActor> cut;
    cut->SetMapper(cutterMapper);

    // Add in some surface geometry for interest.
    vtkNew<vtkContourFilter> iso;
    iso->SetInputData(pl3dOutput);
    iso->SetValue(0, .22);

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(iso->GetOutputPort());
    //normals->SetFeatureAngle(60);

    vtkNew<vtkPolyDataMapper> isoMapper;
    isoMapper->SetInputConnection(iso->GetOutputPort());
    //isoMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> isoActor;
    isoActor->SetMapper(isoMapper);
    isoActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Tomato").GetData());
    isoActor->GetProperty()->SetSpecularColor(
        colors->GetColor3d("White").GetData());
    isoActor->GetProperty()->SetDiffuse(.8);
    isoActor->GetProperty()->SetSpecular(.5);
    isoActor->GetProperty()->SetSpecularPower(30);

 
    // Create the RenderWindow, Renderer and Interactor
    //
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    vtkNew<vtkRenderWindowInteractor> iren;

    iren->SetRenderWindow(renWin);

  
    //ren1->AddActor(isoActor);
    isoActor->VisibilityOn();
    ren1->AddActor(cut);

    unsigned int n = 100;
    double opacity = 1.0 / (static_cast<double>(n)) * 5.0;
    cut->GetProperty()->SetOpacity(1);
    ren1->SetBackground(colors->GetColor3d("Slategray").GetData());

    renWin->SetSize(640, 480);
    renWin->SetWindowName("PseudoVolumeRendering");

    ren1->GetActiveCamera()->SetClippingRange(3.95297, 50);
    ren1->GetActiveCamera()->SetFocalPoint(9.71821, 0.458166, 29.3999);
    ren1->GetActiveCamera()->SetPosition(2.7439, -37.3196, 38.7167);
    ren1->GetActiveCamera()->ComputeViewPlaneNormal();
    ren1->GetActiveCamera()->SetViewUp(-0.16123, 0.264271, 0.950876);

    // Cut: generates n cut planes normal to camera's view plane
    //
    plane->SetNormal(ren1->GetActiveCamera()->GetViewPlaneNormal());
    plane->SetOrigin(ren1->GetActiveCamera()->GetFocalPoint());
    cutter->GenerateValues(n, -5, 5);
    clut->SetAlphaRange(opacity, opacity);
    renWin->Render();

    iren->Start();

    return EXIT_SUCCESS;
  }
