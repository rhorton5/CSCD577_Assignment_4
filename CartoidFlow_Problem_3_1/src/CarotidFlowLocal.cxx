#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkOutlineFilter.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkThresholdPoints.h>
#include <vtkTubeFilter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkScalarBarActor.h>
#include <vtkThresholdPoints.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>

int main(int argc, char* argv[]) {

    float z;
    printf("Please select a z value: ");
    cin >> z;

    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren1);

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    // create pipeline
    //
    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName("velocity.dat");
    reader->Update();

    vtkSmartPointer<vtkPointSource> psource = vtkSmartPointer<vtkPointSource>::New();
    psource->SetNumberOfPoints(25);
    psource->SetCenter(9,9,z);
    psource->SetRadius(2.0);

    vtkSmartPointer<vtkThresholdPoints> threshold = vtkSmartPointer<vtkThresholdPoints>::New();
    threshold->SetInputConnection(reader->GetOutputPort());
    threshold->ThresholdByUpper(275);


    vtkSmartPointer<vtkStreamTracer> streamers = vtkSmartPointer<vtkStreamTracer>::New();
    streamers->SetInputConnection(reader->GetOutputPort());
    streamers->SetSourceConnection(psource->GetOutputPort());

    streamers->SetIntegratorTypeToRungeKutta4();
    streamers->SetMaximumPropagation(100.0);

    streamers->SetInitialIntegrationStep(0.2);
    streamers->SetTerminalSpeed(.01);
    streamers->Update();


    double range[2];
    range[0] = streamers->GetOutput()->GetPointData()->GetScalars()->GetRange()[0];
    range[1] = streamers->GetOutput()->GetPointData()->GetScalars()->GetRange()[1];

    printf("Range[0] is %f\n", range[0]);
    printf("Range[1] is %f\n", range[1]);

    vtkSmartPointer<vtkTubeFilter> tubes = vtkSmartPointer<vtkTubeFilter>::New();
    tubes->SetInputConnection(streamers->GetOutputPort());
    tubes->SetRadius(0.03);
    tubes->SetNumberOfSides(6);
    tubes->SetVaryRadius(1);

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetHueRange(.667, 0.0);
    lut->Build();

    vtkSmartPointer<vtkPolyDataMapper> streamerMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    streamerMapper->SetInputConnection(tubes->GetOutputPort());
    streamerMapper->SetScalarRange(range[0], range[1]);
    streamerMapper->SetLookupTable(lut);

    vtkSmartPointer<vtkActor> streamerActor = vtkSmartPointer<vtkActor>::New();
    streamerActor->SetMapper(streamerMapper);

    // contours of speed
    vtkSmartPointer<vtkContourFilter> iso = vtkSmartPointer<vtkContourFilter>::New();
    iso->SetInputConnection(reader->GetOutputPort());
    iso->SetValue(0, 0.5);

    vtkSmartPointer<vtkPolyDataMapper> isoMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    isoMapper->SetInputConnection(iso->GetOutputPort());
    isoMapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkActor> isoActor = vtkSmartPointer<vtkActor>::New();
    isoActor->SetMapper(isoMapper);
    // isoActor->GetProperty()->SetRepresentationToWireframe();
    isoActor->GetProperty()->SetOpacity(0.25);

    // outline
    vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
    outline->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> outlineMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

    // Add the actors to the renderer, set the background and size
    //
    ren1->AddActor(outlineActor);
    ren1->AddActor(streamerActor);
    ren1->AddActor(isoActor);
    ren1->SetBackground(colors->GetColor3d("Wheat").GetData());
    renWin->SetSize(640, 480);

    //Setup Camera
    vtkSmartPointer<vtkCamera> cam1 = vtkSmartPointer<vtkCamera>::New();
    cam1->SetViewUp(0, 1, 0);
    cam1->SetPosition(0, 0, 40);
    cam1->SetFocalPoint(9, 9, 5);
    ren1->SetActiveCamera(cam1);

    //Create Text
    vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    char title[50], res[50];
    sprintf_s(title, "Velocity At A Boundary When Z = %.2f", z);
    textActor->SetInput(title);
    textActor->SetPosition(100, 20);
    textActor->GetTextProperty()->SetFontSize(24);
    textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);

    //Create Axes
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

    //X-Axis
    axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 0);
    axes->SetXAxisLabelText("X");
    axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(35);

    //Y-Axis
    axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 0);
    axes->SetYAxisLabelText("Y");
    axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(35);

    //Z-Axis
    axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 0);
    axes->SetZAxisLabelText("Z");
    axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(35);

    //
    axes->SetTotalLength(10, 10, 10);

    //HueLut
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetTableRange(0, 1);
    hueLut->SetHueRange(0.0, 0.667);
    hueLut->SetSaturationRange(1, 1);
    hueLut->SetValueRange(1, 1);
    hueLut->Build();

    //Scalar Bar
    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->UnconstrainedFontSizeOn();
    scalarBar->SetTitle("Velocity");
    scalarBar->SetNumberOfLabels(4);
    scalarBar->SetLookupTable(hueLut);
    scalarBar->GetPositionCoordinate()->SetValue(.125, .125);
    scalarBar->SetWidth(0.125);
    scalarBar->SetHeight(0.75);

    //Set up Sphere
    vtkSphereSource* sphere = vtkSphereSource::New();
    sphere->SetRadius(0.5);
    sphere->SetThetaResolution(100);
    sphere->SetPhiResolution(100);
    vtkPolyDataMapper* sphereMapper = vtkPolyDataMapper::New();
    sphereMapper->SetInputConnection(sphere->GetOutputPort());
    vtkActor* sphereActor = vtkActor::New();
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(0.2, 0.63, 0.79);
    sphereActor->SetPosition(9,9,z);

    //Render All Actors
    ren1->AddActor(textActor);
    ren1->AddActor(axes);
    ren1->AddActor(scalarBar);
    ren1->AddActor(sphereActor);

    // render the image
    //
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}
