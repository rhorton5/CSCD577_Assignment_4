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
#include <vtkConeSource.h>
#include <vtkTubeFilter.h>
#include <vtkMaskPoints.h>
#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>
#include <vtkHedgeHog.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkScalarBarActor.h>


int main (int argc, char *argv[]){

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

								 
  vtkSmartPointer<vtkThresholdPoints> threshold = vtkSmartPointer<vtkThresholdPoints>::New();
                                      threshold->SetInputConnection(reader->GetOutputPort());
                                      threshold->ThresholdByUpper(0);

 vtkSmartPointer<vtkMaskPoints> mask = vtkSmartPointer<vtkMaskPoints>::New();
									  mask->SetInputConnection(threshold->GetOutputPort());
									  mask->SetOnRatio(10);
									  mask->Update();

vtkSmartPointer<vtkArrowSource>  arrow = vtkSmartPointer<vtkArrowSource>::New();
							     arrow->SetTipResolution(6);
								 arrow->SetTipRadius(0.2);
								 arrow->SetTipLength(0.4);
								 arrow->SetShaftResolution(6);
								 arrow->SetShaftRadius(0.075);


vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
							   cone->SetResolution(3);
							   cone->SetHeight(1);
							   cone->SetRadius(0.15);

vtkSmartPointer<vtkHedgeHog> hhog = vtkSmartPointer<vtkHedgeHog>::New();
							 hhog->SetInputConnection(threshold->GetOutputPort());
							 hhog->SetScaleFactor(0.5);

vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
							   glyph->SetInputConnection(threshold->GetOutputPort());
							   glyph->SetSourceConnection(arrow->GetOutputPort());
							   glyph->SetScaleFactor(1.5);
							   glyph->SetScaleModeToScaleByScalar();
							   glyph->SetVectorModeToUseVector();
							   glyph->OrientOn();



  double range[2];
  range[0] = threshold->GetOutput()->GetPointData()->GetScalars()->GetRange()[0];
  range[1] = threshold->GetOutput()->GetPointData()->GetScalars()->GetRange()[1];

  printf("Range[0] is %f\n", range[0]);
  printf("Range[1] is %f\n", range[1]);


 vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
                                  lut->SetHueRange(.667, 0.0);
                                  lut->Build();

vtkSmartPointer<vtkPolyDataMapper> vecMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
								 vecMapper->SetInputConnection(glyph->GetOutputPort());
                                 // vecMapper->SetInputConnection(hhog->GetOutputPort());
								  vecMapper->SetScalarRange(range[0], range[1]);
								  vecMapper->SetLookupTable(lut);

 

 vtkSmartPointer<vtkActor> streamerActor = vtkSmartPointer<vtkActor>::New();
                            streamerActor->SetMapper(vecMapper);


// outline
  vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
                                    outline->SetInputConnection(reader->GetOutputPort());

  vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                                     outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
                            outlineActor->SetMapper(outlineMapper);
                            outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

// Add the actors to the renderer, set the background and size
//
  ren1->AddActor(outlineActor);
  ren1->AddActor(streamerActor);
  ren1->SetBackground(colors->GetColor3d("Wheat").GetData());
  renWin->SetSize(640, 480);

  vtkSmartPointer<vtkCamera> cam1 = vtkSmartPointer<vtkCamera>::New();
							  cam1->SetViewUp(0, 1, 0);
							  cam1->SetPosition(0, 0, 50);
							  cam1->SetFocalPoint(9, 9, 5);
                             ren1->SetActiveCamera(cam1);

//Create Text
vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
textActor->SetInput("Global Velocity Distribution");
textActor->SetPosition(100, 20);
textActor->GetTextProperty()->SetFontSize(24);
textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);

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

//Render All Actors
ren1->AddActor(textActor);
ren1->AddActor(axes);
ren1->AddActor(scalarBar);

// render the image
//
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
