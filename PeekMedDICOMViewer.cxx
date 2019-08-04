#include "vtkBoxWidget.h"
#include "vtkPlanes.h"
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkSmartPointer.h>
#include <vtkColorTransferFunction.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkNamedColors.h>


// Callback for moving the planes from the box widget to the mapper
class vtkBoxWidgetCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCallback *New()
    { return new vtkBoxWidgetCallback; }
  void Execute(vtkObject *caller, unsigned long, void*) override
  {
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
      if (this->Mapper)
      {
        vtkPlanes *planes = vtkPlanes::New();
        widget->GetPlanes(planes);
        this->Mapper->SetClippingPlanes(planes);
        planes->Delete();
      }
  }
  void SetMapper(vtkFixedPointVolumeRayCastMapper* m)
    { this->Mapper = m; }

protected:
  vtkBoxWidgetCallback()
    { this->Mapper = nullptr; }

  vtkFixedPointVolumeRayCastMapper *Mapper;
};


int main(int argc, char *argv[])
{
  int count = 1;
  char *dirname = NULL;
  int clip = 0;

  while ( count < argc )
  {
    if ( !strcmp( argv[count], "?" ) )
    {
      cout << "Usage: " << argv[0] << "-DICOM <dicom folder>" << "Optional" << "-Clip" << endl;
      exit(EXIT_SUCCESS);
    }
    else if ( !strcmp( argv[count], "-DICOM" ) )
    {
      size_t size = strlen(argv[count+1])+1;
      dirname = new char[size];
      snprintf( dirname, size, "%s", argv[count+1] );
      count += 2;
    }
    else if ( !strcmp( argv[count], "-Clip") )
    {
      clip = 1;
      count++;
    }
    else
    {
      cout << "Unrecognized option: " << argv[count] << endl;
      cout << endl;
      exit(EXIT_FAILURE);
    }
  }


  // Read DICOM images
  vtkSmartPointer<vtkAlgorithm> reader =
    vtkSmartPointer<vtkAlgorithm>::New();
  vtkSmartPointer<vtkImageData> input =
    vtkSmartPointer<vtkImageData>::New();

  vtkSmartPointer<vtkDICOMImageReader> dicomReader =
    vtkSmartPointer<vtkDICOMImageReader>::New();
  dicomReader->SetDirectoryName(dirname);
  dicomReader->Update();
  input=dicomReader->GetOutput();
  reader=dicomReader;

  // Create our transfer function
  vtkSmartPointer<vtkColorTransferFunction> colorFun =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  vtkSmartPointer<vtkPiecewiseFunction> opacityFun =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  vtkSmartPointer<vtkPiecewiseFunction> opacityGradientFun =
    vtkSmartPointer<vtkPiecewiseFunction>::New();

  colorFun->AddRGBPoint(-550.0, 0.08, 0.05, 0.03);
  colorFun->AddRGBPoint(-350.0, 0.39, 0.25, 0.16);
  colorFun->AddRGBPoint(-200.0, 0.80, 0.80, 0.80);
  colorFun->AddRGBPoint(2750.0, 0.70, 0.70, 0.70);
  colorFun->AddRGBPoint(3000.0, 0.35, 0.35, 0.35);

  opacityFun->AddPoint(-800.0, 0.0);
  opacityFun->AddPoint(-750.0, 0.0);
  opacityFun->AddPoint(-350.0, 1.0);
  opacityFun->AddPoint(-300.0, 1.0);
  opacityFun->AddPoint(-200.0, 1.0);
  opacityFun->AddPoint(-100.0, 1.0);
  opacityFun->AddPoint(1000.0, 0.0);
  opacityFun->AddPoint(2750.0, 0.0);
  opacityFun->AddPoint(2976.0, 1.0);
  opacityFun->AddPoint(3000.0, 0.0);

  opacityGradientFun->AddPoint(0.0, 0.0);
  opacityGradientFun->AddPoint(1500.0, 1.0);


  // Create the property and attach the transfer functions
  vtkSmartPointer<vtkVolumeProperty> property =
    vtkSmartPointer<vtkVolumeProperty>::New();
  property->ShadeOn();
  property->SetColor( colorFun );
  property->SetScalarOpacity( opacityFun );
  property->SetGradientOpacity( opacityGradientFun);
  property->SetInterpolationTypeToLinear();

  property->SetAmbient(0.1);
  property->SetDiffuse(0.9);
  property->SetSpecular(0.2);
  property->SetSpecularPower(10.0);

  // Create our volume and mapper
  vtkSmartPointer<vtkVolume> volume =
    vtkSmartPointer<vtkVolume>::New();
  vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper =
    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();

  mapper->SetBlendModeToComposite();
  mapper->SetInputConnection( reader->GetOutputPort() );
  volume->SetMapper( mapper );
  volume->SetProperty( property );

  // Sampling
  double spacing[3];
  input->GetSpacing(spacing);
  // for (int i = 2; i >= 0; i--)
  //   std::cout << spacing[i] << endl;
  mapper->SetSampleDistance( (spacing[0]+spacing[1]+spacing[2])/12.0 );

  // Create the renderer, render window and interactor
  vtkSmartPointer<vtkNamedColors> colors =
    vtkSmartPointer<vtkNamedColors>::New();
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer(renderer);


  // Connect it all.
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  iren->GetInteractorStyle();


  // Creat clipper
  vtkBoxWidget *box = vtkBoxWidget::New();
  if (clip)
  {
    box->SetInteractor(iren);
    box->SetPlaceFactor(1.01);
    box->SetInputData(input);
    box->SetDefaultRenderer(renderer);
    box->InsideOutOn();
    box->PlaceWidget();
    vtkBoxWidgetCallback *callback = vtkBoxWidgetCallback::New();
    callback->SetMapper(mapper);
    box->AddObserver(vtkCommand::InteractionEvent, callback);
    callback->Delete();
    box->EnabledOn();
    box->GetSelectedFaceProperty()->SetOpacity(0.0);
  }


  // Set the default window size
  renWin->SetSize(600,600);
  renWin->Render();

  // Add the volume to the scene
  renderer->AddVolume( volume );
  renderer->ResetCamera();
  renderer->SetBackground(0.1, 0.2, 0.3);

  // interact with data
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;;
}
