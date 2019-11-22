#pragma once
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <random>

struct pData {
double X;
double Y;
int ClassKind;
};
namespace machinelearning {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::Runtime::InteropServices;
	using namespace std;
	/// <summary>
	/// Form1 ªººK­n
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Bitmap^ myBitmap;
		Graphics^ g;
		Brush^ bshDraw;
		Pen^ penDraw;
		String^ Filename1;
		unsigned char PointSize, PointSize1, PointSize2, Distribution;
		pData* InputData;
		double Pi, CenterX, CenterY;
		int ClassKind, MethodCodeValue, NumberOfData, NumberOfPoint, MaxSizeOfData;
		int imW, imH, X_Cur,Y_Cur, RangeX, RangeY, NumOfCluster, NumOfClass, NumClass1, NumClass2;
		bool HandFlag;
		//Bayesian Parameters
		double MeanX1, MeanY1, Sigma2X1, Sigma2Y1, SigmaX1, SigmaY1, SigmaXY1, detA1, Correlation1, Correlation12, PClass1, *PxyClass1;
		double MeanX2, MeanY2, Sigma2X2, Sigma2Y2, SigmaX2, SigmaY2, SigmaXY2, detA2, Correlation2, Correlation22, PClass2, *PxyClass2;

		//Regression--Linear
		double LR_a1, LR_a0;
		//Regression--Nonlinear
		double **A, *B, *NLcoef; //AX=B ==> solve equation a0+a1X+...+adX^d for (a0,a1,...,ad)=NLcoef[]
		int NLdegree; //polynomial degree


	private: System::Windows::Forms::ToolStripMenuItem^  clearToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  showToolStripMenuItem;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::GroupBox^  Classification;
	private: System::Windows::Forms::ComboBox^  comboBox_classify;

	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::CheckBox^  checkBox_Unbiased;

	private: System::Windows::Forms::ToolStripMenuItem^  showDataToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  showResultToolStripMenuItem;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::GroupBox^  clustering;
	private: System::Windows::Forms::ComboBox^  comboBox_clusters;
	private: System::Windows::Forms::GroupBox^  regression;
	private: System::Windows::Forms::ComboBox^  comboBox_regression;
	private: System::Windows::Forms::ToolStripMenuItem^  showContourToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  showMeansToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  showRegressionToolStripMenuItem;
	private: System::Windows::Forms::ComboBox^  comboBox_NL_degree;
	private: System::Windows::Forms::ComboBox^  comboBox_clustering;


	public:
		Brush^ ClassToColor(int c) {
			Brush^ gcBrush;
		switch (c)
			{
            case -2:
				gcBrush = gcnew SolidBrush(Color::Black);
                break;
            case -1:
 				gcBrush = gcnew SolidBrush(Color::Blue);
                break;
            case 0:
				gcBrush = gcnew SolidBrush(Color::Green);
                break;
            case 1:
				gcBrush = gcnew SolidBrush(Color::Red);
                break;
            default:
				gcBrush = gcnew SolidBrush(Color::Purple);
                break;
				 }
		return gcBrush;
		}
		Pen^ ClassToPenColor(int c) {
				Pen^ gcPen;
		switch (c)
			{
				case -2:
						gcPen = gcnew Pen(Color::Black);
						break;
				case -1:
 						gcPen = gcnew Pen(Color::Blue);
						break;
				case 0:
						gcPen = gcnew Pen(Color::Green);
						break;
				case 1:
						gcPen = gcnew Pen(Color::Red);
						break;
				default:
						gcPen = gcnew Pen(Color::Purple);
				break;
				 }
			return gcPen;
			}
		void BayesMAP(){
			CalculateMeanSigma2();
			CalculateBayesianProb();
		}
		void NewPublicVariables(int MaxNumberOfData){
			InputData = new pData[MaxNumberOfData];
			PxyClass1 = new double[MaxNumberOfData];
			PxyClass2 = new double[MaxNumberOfData];
		} 
		void DeletePublicVariables(int a){
			delete[] InputData;
			delete[] PxyClass1;
			delete[] PxyClass2;
		}
		double Sgn(double Num1){
			return (Num1 >= 0.0) ? 1.0 : -1.0;
		}
		void CalculateMeanSigma2( ){
			MeanX1=0.0;MeanY1=0.0;MeanX2=0.0;MeanY2=0.0;
			NumClass1=0;NumClass2=0;
			for(int i=0;i<NumberOfData;i++){
				if(InputData[i].ClassKind==1){
					MeanX1+=InputData[i].X;
					MeanY1+=InputData[i].Y;
					NumClass1++;
				}
				else{
					MeanX2+=InputData[i].X;
					MeanY2+=InputData[i].Y;
					NumClass2++;
				}}
				MeanX1=MeanX1/NumClass1;MeanY1=MeanY1/NumClass1;
				MeanX2=MeanX2/NumClass2;MeanY2=MeanY2/NumClass2;

				Sigma2X1=0.0;Sigma2Y1=0.0;
				Sigma2X2=0.0;Sigma2Y2=0.0;
				SigmaXY1=0.0;SigmaXY2=0.0;
			for(int i=0;i<NumberOfData;i++){
				if(InputData[i].ClassKind==1){
					Sigma2X1 += pow((InputData[i].X-MeanX1),2);
					Sigma2Y1 += pow((InputData[i].Y-MeanY1),2);
					SigmaXY1 += (InputData[i].X-MeanX1)* (InputData[i].Y-MeanY1);
				}
				else{
					Sigma2X2 += pow((InputData[i].X-MeanX2),2);
					Sigma2Y2 += pow((InputData[i].Y-MeanY2),2);
					SigmaXY2 += (InputData[i].X-MeanX2)* (InputData[i].Y-MeanY2);
				}}

				if(NumClass1>0){
					if(NumClass1==1 || !checkBox_Unbiased->Checked){
					Sigma2X1/=NumClass1;
					Sigma2Y1/=NumClass1;
					SigmaXY1/=NumClass1;}
					else{
					Sigma2X1/=(NumClass1-1);
					Sigma2Y1/=(NumClass1-1);
					SigmaXY1/=(NumClass1-1);
					}}
					if(NumClass2==1 || !checkBox_Unbiased->Checked){
					Sigma2X2/=NumClass2;
					Sigma2Y2/=NumClass2;
					SigmaXY2/=NumClass2;
					}
					else{
					Sigma2X2/=(NumClass2-1);
					Sigma2Y2/=(NumClass2-1);
					SigmaXY2/=(NumClass2-1);
					}
			
					SigmaX1=sqrt(Sigma2X1);SigmaY1=sqrt(Sigma2Y1);
					SigmaX2=sqrt(Sigma2X2);SigmaY2=sqrt(Sigma2Y2);
					Correlation1 = SigmaXY1/(SigmaX1*SigmaY1);Correlation12 = Correlation1*Correlation1;
					Correlation2 = SigmaXY2/(SigmaX2*SigmaY2);Correlation22 = Correlation2*Correlation2;
					detA1=Sigma2X1*Sigma2Y1-SigmaXY1*SigmaXY1;
					detA2=Sigma2X2*Sigma2Y2-SigmaXY2*SigmaXY2;
			}
		double evalPxy1(pData Data1){
			double dx,dy,dx2,dy2,c1,Ndist,tmp;


			dx=Data1.X - MeanX1;dy=Data1.Y - MeanY1;
			dx2=dx*dx;dy2=dy*dy;

			if (Correlation12 != 1.0){
				c1=1.0/(1.0-Correlation12);
			}
			else{c1=1.0;}

			if(detA1==0.0){
				Ndist=dx2 - 2.0*dx*dy +dy2;
				tmp=exp(-0.5*Ndist);

			}
			else{
				Ndist=dx2/Sigma2X1-2.0*Correlation12*dx*dy/SigmaXY1+dy2/Sigma2Y1;
				tmp=exp(-0.5*c1*Ndist)/(2.0*Pi*sqrt(detA1));
			}
			return tmp;
		}
		double evalPxy2(pData Data1){
			double dx,dy,dx2,dy2,c1,Ndist,tmp;

			
			dx=Data1.X - MeanX2;dy=Data1.Y - MeanY2;
			dx2=dx*dx;dy2=dy*dy;

			if (Correlation22 != 1.0){
				c1=1.0/(1.0-Correlation22);
			}
			else{c1=1.0;}

			if(detA2==0.0){
				Ndist=dx2 - 2.0*dx*dy +dy2;
				tmp=exp(-0.5*Ndist);

			}
			else{
				Ndist=dx2/Sigma2X2-2.0*Correlation22*dx*dy/SigmaXY2+dy2/Sigma2Y2;
				tmp=exp(-0.5*c1*Ndist)/(2.0*Pi*sqrt(detA2));
			}
			return tmp;
		}
		void CalculateBayesianProb( ){
				double dx,dy,dx2,dy2,c1,Ndist;

				PClass1=(double)NumClass1/NumberOfData;
				PClass2=(double)1.0 -PClass1;

			for(int i=0;i<NumberOfData;i++){				
				dx =InputData[i].X-MeanX1; dy =InputData[i].Y-MeanY1;
				dx2=dx*dx; dy2=dy*dy;

				if (Correlation12 != 1.0){
					c1=1.0/(1.0-Correlation12);}
				else c1=1.0;

				if(detA1==0.0){
				Ndist=dx2-2.0*dx*dy+dy2;
				PxyClass1[i]=exp(-0.5*Ndist);
				}
				else{
				Ndist=dx2/Sigma2X1-2.0*Correlation12*dx*dy/SigmaXY1+dy2/Sigma2Y1;
				PxyClass1[i]=exp(-0.5*c1*Ndist)/(2.0*Pi*sqrt(detA1));
				}
				dx =InputData[i].X-MeanX2; dy =InputData[i].Y-MeanY2;
				dx2=dx*dx; dy2=dy*dy;

				if (Correlation22 != 1.0){
					c1=1.0/(1.0-Correlation22);}
				else c1=1.0;

				if(detA2==0.0){
				Ndist=dx2-2.0*dx*dy+dy2;
				PxyClass2[i]=exp(-0.5*Ndist);
				}
				else{
				Ndist=dx2/Sigma2X2-2.0*Correlation22*dx*dy/SigmaXY2+dy2/Sigma2Y2;
				PxyClass2[i]=exp(-0.5*c1*Ndist)/(2.0*Pi*sqrt(detA2));
				}
			}
		}
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: ¦b¦¹¥[¤J«Øºc¨ç¦¡µ{¦¡½X
			//
		}

	protected:
		/// <summary>
		/// ²M°£¥ô¦ó¨Ï¥Î¤¤ªº¸ê·½¡C
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	protected: 
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  imagToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  dataEditToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  applicationToolStripMenuItem;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton1;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton2;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton3;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton4;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton5;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton6;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton7;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton8;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton9;
	private: System::Windows::Forms::ToolStripButton^  toolStripButton10;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox_X;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBox_Y;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ComboBox^  comboBox_psize;

	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::TextBox^  textBox_datasize;

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TextBox^  textBox_MaxSize;

	private: System::Windows::Forms::Button^  button_Clear;


	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::GroupBox^  Run;
private: System::Windows::Forms::ComboBox^  comboBox_Run;




	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::RadioButton^  radioButton_Group;

	private: System::Windows::Forms::RadioButton^  radioButton_Single;

	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::ComboBox^  comboBox_CS;
	private: System::Windows::Forms::RadioButton^  radioButton_S1;
	private: System::Windows::Forms::RadioButton^  radioButton_NC;




	private: System::Windows::Forms::RadioButton^  radioButton_C2;

	private: System::Windows::Forms::RadioButton^  radioButton_C1;

	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::RadioButton^  radioButton8;
	private: System::Windows::Forms::RadioButton^  radioButton7;
	private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::TextBox^  Y;

private: System::Windows::Forms::TextBox^  X;



	private: System::Windows::Forms::TextBox^  textBox5;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::HScrollBar^  hScrollBar1;
	private: System::Windows::Forms::TextBox^  textBox8;
	private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::TextBox^  textBox_Filename;

	private: System::Windows::Forms::RichTextBox^  richTextBox1;
	private: System::Windows::Forms::FontDialog^  fontDialog1;

	/*private: System::Void pictureBox1_MouseMove(System::Object^ sender,System::Windows::Forms::MouseEventArgs^ e){}
	private: System::Void pictureBox1_MouseClick(System::Object^ sender,System::Windows::Forms::MouseEventArgs^ e){}
	private: System::Void pictureBox1_MouseDown(System::Object^ sender,System::Windows::Forms::MouseEventArgs^ e){}*/


	private:
		/// <summary>
		/// ³]­p¤u¨ã©Ò»ÝªºÅÜ¼Æ¡C
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// ¦¹¬°³]­p¤u¨ã¤ä´©©Ò»Ýªº¤èªk - ½Ð¤Å¨Ï¥Îµ{¦¡½X½s¿è¾¹
		/// ­×§ï³o­Ó¤èªkªº¤º®e¡C
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->imagToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showDataToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->clearToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showResultToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showContourToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showMeansToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showRegressionToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dataEditToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->applicationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton2 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton3 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton4 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton5 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton6 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton7 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton8 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton9 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton10 = (gcnew System::Windows::Forms::ToolStripButton());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox_X = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBox_Y = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_psize = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox_datasize = (gcnew System::Windows::Forms::TextBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox_MaxSize = (gcnew System::Windows::Forms::TextBox());
			this->button_Clear = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->Run = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_Run = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_Group = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_Single = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_CS = (gcnew System::Windows::Forms::ComboBox());
			this->radioButton_S1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_NC = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_C2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_C1 = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->hScrollBar1 = (gcnew System::Windows::Forms::HScrollBar());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton8 = (gcnew System::Windows::Forms::RadioButton());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->textBox8 = (gcnew System::Windows::Forms::TextBox());
			this->radioButton7 = (gcnew System::Windows::Forms::RadioButton());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->Y = (gcnew System::Windows::Forms::TextBox());
			this->textBox5 = (gcnew System::Windows::Forms::TextBox());
			this->X = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->textBox_Filename = (gcnew System::Windows::Forms::TextBox());
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			this->fontDialog1 = (gcnew System::Windows::Forms::FontDialog());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->Classification = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_classify = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->checkBox_Unbiased = (gcnew System::Windows::Forms::CheckBox());
			this->clustering = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_clusters = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_clustering = (gcnew System::Windows::Forms::ComboBox());
			this->regression = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_NL_degree = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_regression = (gcnew System::Windows::Forms::ComboBox());
			this->menuStrip1->SuspendLayout();
			this->toolStrip1->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->Run->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->Classification->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->clustering->SuspendLayout();
			this->regression->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->fileToolStripMenuItem, 
				this->imagToolStripMenuItem, this->dataEditToolStripMenuItem, this->aboutToolStripMenuItem, this->applicationToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(864, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->openToolStripMenuItem, 
				this->saveToolStripMenuItem, this->showToolStripMenuItem, this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(38, 20);
			this->fileToolStripMenuItem->Text = L"File";
			this->fileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::fileToolStripMenuItem_Click);
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(135, 22);
			this->openToolStripMenuItem->Text = L"Open";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(135, 22);
			this->saveToolStripMenuItem->Text = L"Save";
			this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveToolStripMenuItem_Click);
			// 
			// showToolStripMenuItem
			// 
			this->showToolStripMenuItem->Name = L"showToolStripMenuItem";
			this->showToolStripMenuItem->Size = System::Drawing::Size(135, 22);
			this->showToolStripMenuItem->Text = L"Show Data";
			this->showToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(135, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// imagToolStripMenuItem
			// 
			this->imagToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->showDataToolStripMenuItem, 
				this->clearToolStripMenuItem, this->showResultToolStripMenuItem, this->showContourToolStripMenuItem, this->showMeansToolStripMenuItem, 
				this->showRegressionToolStripMenuItem});
			this->imagToolStripMenuItem->Name = L"imagToolStripMenuItem";
			this->imagToolStripMenuItem->Size = System::Drawing::Size(66, 20);
			this->imagToolStripMenuItem->Text = L"Imaging";
			// 
			// showDataToolStripMenuItem
			// 
			this->showDataToolStripMenuItem->Name = L"showDataToolStripMenuItem";
			this->showDataToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->showDataToolStripMenuItem->Text = L"Show Data";
			this->showDataToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showDataToolStripMenuItem_Click);
			// 
			// clearToolStripMenuItem
			// 
			this->clearToolStripMenuItem->Name = L"clearToolStripMenuItem";
			this->clearToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->clearToolStripMenuItem->Text = L"Clear image";
			this->clearToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::clearToolStripMenuItem_Click);
			// 
			// showResultToolStripMenuItem
			// 
			this->showResultToolStripMenuItem->Name = L"showResultToolStripMenuItem";
			this->showResultToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->showResultToolStripMenuItem->Text = L"Show Result";
			this->showResultToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showResultToolStripMenuItem_Click);
			// 
			// showContourToolStripMenuItem
			// 
			this->showContourToolStripMenuItem->Name = L"showContourToolStripMenuItem";
			this->showContourToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->showContourToolStripMenuItem->Text = L"Show Contour";
			this->showContourToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showContourToolStripMenuItem_Click);
			// 
			// showMeansToolStripMenuItem
			// 
			this->showMeansToolStripMenuItem->Name = L"showMeansToolStripMenuItem";
			this->showMeansToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->showMeansToolStripMenuItem->Text = L"Show Means";
			this->showMeansToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showMeansToolStripMenuItem_Click);
			// 
			// showRegressionToolStripMenuItem
			// 
			this->showRegressionToolStripMenuItem->Name = L"showRegressionToolStripMenuItem";
			this->showRegressionToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->showRegressionToolStripMenuItem->Text = L"Show Regression";
			this->showRegressionToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showRegressionToolStripMenuItem_Click);
			// 
			// dataEditToolStripMenuItem
			// 
			this->dataEditToolStripMenuItem->Name = L"dataEditToolStripMenuItem";
			this->dataEditToolStripMenuItem->Size = System::Drawing::Size(68, 20);
			this->dataEditToolStripMenuItem->Text = L"DataEdit";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(54, 20);
			this->aboutToolStripMenuItem->Text = L"About";
			// 
			// applicationToolStripMenuItem
			// 
			this->applicationToolStripMenuItem->Name = L"applicationToolStripMenuItem";
			this->applicationToolStripMenuItem->Size = System::Drawing::Size(89, 20);
			this->applicationToolStripMenuItem->Text = L"Applications";
			// 
			// toolStrip1
			// 
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(10) {this->toolStripButton1, 
				this->toolStripButton2, this->toolStripButton3, this->toolStripButton4, this->toolStripButton5, this->toolStripButton6, this->toolStripButton7, 
				this->toolStripButton8, this->toolStripButton9, this->toolStripButton10});
			this->toolStrip1->Location = System::Drawing::Point(0, 24);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(864, 25);
			this->toolStrip1->TabIndex = 1;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// toolStripButton1
			// 
			this->toolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton1.Image")));
			this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton1->Name = L"toolStripButton1";
			this->toolStripButton1->Size = System::Drawing::Size(23, 22);
			this->toolStripButton1->Text = L"toolStripButton1";
			// 
			// toolStripButton2
			// 
			this->toolStripButton2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton2.Image")));
			this->toolStripButton2->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton2->Name = L"toolStripButton2";
			this->toolStripButton2->Size = System::Drawing::Size(23, 22);
			this->toolStripButton2->Text = L"toolStripButton2";
			this->toolStripButton2->Click += gcnew System::EventHandler(this, &Form1::toolStripButton2_Click);
			// 
			// toolStripButton3
			// 
			this->toolStripButton3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton3.Image")));
			this->toolStripButton3->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton3->Name = L"toolStripButton3";
			this->toolStripButton3->Size = System::Drawing::Size(23, 22);
			this->toolStripButton3->Text = L"toolStripButton3";
			this->toolStripButton3->Click += gcnew System::EventHandler(this, &Form1::toolStripButton3_Click);
			// 
			// toolStripButton4
			// 
			this->toolStripButton4->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton4->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton4.Image")));
			this->toolStripButton4->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton4->Name = L"toolStripButton4";
			this->toolStripButton4->Size = System::Drawing::Size(23, 22);
			this->toolStripButton4->Text = L"toolStripButton4";
			// 
			// toolStripButton5
			// 
			this->toolStripButton5->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton5->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton5.Image")));
			this->toolStripButton5->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton5->Name = L"toolStripButton5";
			this->toolStripButton5->Size = System::Drawing::Size(23, 22);
			this->toolStripButton5->Text = L"toolStripButton5";
			// 
			// toolStripButton6
			// 
			this->toolStripButton6->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton6->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton6.Image")));
			this->toolStripButton6->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton6->Name = L"toolStripButton6";
			this->toolStripButton6->Size = System::Drawing::Size(23, 22);
			this->toolStripButton6->Text = L"toolStripButton6";
			// 
			// toolStripButton7
			// 
			this->toolStripButton7->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton7->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton7.Image")));
			this->toolStripButton7->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton7->Name = L"toolStripButton7";
			this->toolStripButton7->Size = System::Drawing::Size(23, 22);
			this->toolStripButton7->Text = L"toolStripButton7";
			// 
			// toolStripButton8
			// 
			this->toolStripButton8->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton8->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton8.Image")));
			this->toolStripButton8->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton8->Name = L"toolStripButton8";
			this->toolStripButton8->Size = System::Drawing::Size(23, 22);
			this->toolStripButton8->Text = L"toolStripButton8";
			// 
			// toolStripButton9
			// 
			this->toolStripButton9->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton9->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton9.Image")));
			this->toolStripButton9->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton9->Name = L"toolStripButton9";
			this->toolStripButton9->Size = System::Drawing::Size(23, 22);
			this->toolStripButton9->Text = L"toolStripButton9";
			// 
			// toolStripButton10
			// 
			this->toolStripButton10->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton10->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton10.Image")));
			this->toolStripButton10->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton10->Name = L"toolStripButton10";
			this->toolStripButton10->Size = System::Drawing::Size(23, 22);
			this->toolStripButton10->Text = L"toolStripButton10";
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(49, 63);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(74, 16);
			this->checkBox1->TabIndex = 2;
			this->checkBox1->Text = L"Show Data";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(47, 92);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(16, 12);
			this->label1->TabIndex = 3;
			this->label1->Text = L"X:";
			// 
			// textBox_X
			// 
			this->textBox_X->Location = System::Drawing::Point(69, 85);
			this->textBox_X->Name = L"textBox_X";
			this->textBox_X->Size = System::Drawing::Size(62, 22);
			this->textBox_X->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(137, 92);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(16, 12);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Y:";
			// 
			// textBox_Y
			// 
			this->textBox_Y->Location = System::Drawing::Point(157, 84);
			this->textBox_Y->Name = L"textBox_Y";
			this->textBox_Y->Size = System::Drawing::Size(58, 22);
			this->textBox_Y->TabIndex = 6;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->comboBox_psize);
			this->groupBox1->Location = System::Drawing::Point(223, 71);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(90, 42);
			this->groupBox1->TabIndex = 7;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"PointSize";
			// 
			// comboBox_psize
			// 
			this->comboBox_psize->FormattingEnabled = true;
			this->comboBox_psize->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"1", L"2", L"3"});
			this->comboBox_psize->Location = System::Drawing::Point(7, 14);
			this->comboBox_psize->Name = L"comboBox_psize";
			this->comboBox_psize->Size = System::Drawing::Size(77, 20);
			this->comboBox_psize->TabIndex = 0;
			this->comboBox_psize->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox_psize_SelectedIndexChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->textBox_datasize);
			this->groupBox2->Location = System::Drawing::Point(319, 71);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(99, 42);
			this->groupBox2->TabIndex = 8;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"DataSize";
			// 
			// textBox_datasize
			// 
			this->textBox_datasize->Location = System::Drawing::Point(6, 14);
			this->textBox_datasize->Name = L"textBox_datasize";
			this->textBox_datasize->Size = System::Drawing::Size(83, 22);
			this->textBox_datasize->TabIndex = 0;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->textBox_MaxSize);
			this->groupBox3->Location = System::Drawing::Point(424, 70);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(98, 42);
			this->groupBox3->TabIndex = 9;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"MaxSize";
			// 
			// textBox_MaxSize
			// 
			this->textBox_MaxSize->Location = System::Drawing::Point(6, 14);
			this->textBox_MaxSize->Name = L"textBox_MaxSize";
			this->textBox_MaxSize->Size = System::Drawing::Size(84, 22);
			this->textBox_MaxSize->TabIndex = 0;
			this->textBox_MaxSize->Text = L"1000";
			this->textBox_MaxSize->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_MaxSize_TextChanged);
			// 
			// button_Clear
			// 
			this->button_Clear->Location = System::Drawing::Point(528, 81);
			this->button_Clear->Name = L"button_Clear";
			this->button_Clear->Size = System::Drawing::Size(83, 30);
			this->button_Clear->TabIndex = 10;
			this->button_Clear->Text = L"Clear";
			this->button_Clear->Click += gcnew System::EventHandler(this, &Form1::button_Clear_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(617, 82);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(91, 30);
			this->button2->TabIndex = 11;
			this->button2->Text = L"Run";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(13, 150);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(20, 12);
			this->label3->TabIndex = 12;
			this->label3->Text = L"1.0";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(13, 393);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(20, 12);
			this->label4->TabIndex = 13;
			this->label4->Text = L"0.0";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(49, 148);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(512, 512);
			this->pictureBox1->TabIndex = 14;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseClick);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_Down);
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_Move);
			// 
			// Run
			// 
			this->Run->Controls->Add(this->comboBox_Run);
			this->Run->Location = System::Drawing::Point(719, 63);
			this->Run->Name = L"Run";
			this->Run->Size = System::Drawing::Size(139, 40);
			this->Run->TabIndex = 15;
			this->Run->TabStop = false;
			this->Run->Text = L"Run Program";
			// 
			// comboBox_Run
			// 
			this->comboBox_Run->FormattingEnabled = true;
			this->comboBox_Run->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Classification", L"Clustering", L"Regression"});
			this->comboBox_Run->Location = System::Drawing::Point(7, 15);
			this->comboBox_Run->Name = L"comboBox_Run";
			this->comboBox_Run->Size = System::Drawing::Size(121, 20);
			this->comboBox_Run->TabIndex = 0;
			this->comboBox_Run->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox_Run_SelectedIndexChanged);
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->radioButton_Group);
			this->groupBox5->Controls->Add(this->radioButton_Single);
			this->groupBox5->Location = System::Drawing::Point(569, 127);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(139, 48);
			this->groupBox5->TabIndex = 16;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Input Mode";
			// 
			// radioButton_Group
			// 
			this->radioButton_Group->AutoSize = true;
			this->radioButton_Group->Location = System::Drawing::Point(65, 21);
			this->radioButton_Group->Name = L"radioButton_Group";
			this->radioButton_Group->Size = System::Drawing::Size(53, 16);
			this->radioButton_Group->TabIndex = 1;
			this->radioButton_Group->Text = L"Group";
			this->radioButton_Group->UseVisualStyleBackColor = true;
			// 
			// radioButton_Single
			// 
			this->radioButton_Single->AutoSize = true;
			this->radioButton_Single->Checked = true;
			this->radioButton_Single->Location = System::Drawing::Point(7, 22);
			this->radioButton_Single->Name = L"radioButton_Single";
			this->radioButton_Single->Size = System::Drawing::Size(52, 16);
			this->radioButton_Single->TabIndex = 0;
			this->radioButton_Single->TabStop = true;
			this->radioButton_Single->Text = L"Single";
			this->radioButton_Single->UseVisualStyleBackColor = true;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->comboBox_CS);
			this->groupBox6->Controls->Add(this->radioButton_S1);
			this->groupBox6->Controls->Add(this->radioButton_NC);
			this->groupBox6->Controls->Add(this->radioButton_C2);
			this->groupBox6->Controls->Add(this->radioButton_C1);
			this->groupBox6->Location = System::Drawing::Point(569, 181);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(139, 101);
			this->groupBox6->TabIndex = 17;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Target";
			// 
			// comboBox_CS
			// 
			this->comboBox_CS->FormattingEnabled = true;
			this->comboBox_CS->Location = System::Drawing::Point(79, 45);
			this->comboBox_CS->Name = L"comboBox_CS";
			this->comboBox_CS->Size = System::Drawing::Size(49, 20);
			this->comboBox_CS->TabIndex = 4;
			this->comboBox_CS->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox_CS_SelectedIndexChanged);
			// 
			// radioButton_S1
			// 
			this->radioButton_S1->AutoSize = true;
			this->radioButton_S1->Location = System::Drawing::Point(78, 21);
			this->radioButton_S1->Name = L"radioButton_S1";
			this->radioButton_S1->Size = System::Drawing::Size(50, 16);
			this->radioButton_S1->TabIndex = 3;
			this->radioButton_S1->TabStop = true;
			this->radioButton_S1->Text = L"Select";
			this->radioButton_S1->UseVisualStyleBackColor = true;
			this->radioButton_S1->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton_S1_CheckedChanged);
			// 
			// radioButton_NC
			// 
			this->radioButton_NC->AutoSize = true;
			this->radioButton_NC->Location = System::Drawing::Point(7, 67);
			this->radioButton_NC->Name = L"radioButton_NC";
			this->radioButton_NC->Size = System::Drawing::Size(64, 16);
			this->radioButton_NC->TabIndex = 2;
			this->radioButton_NC->TabStop = true;
			this->radioButton_NC->Text = L"No Class";
			this->radioButton_NC->UseVisualStyleBackColor = true;
			this->radioButton_NC->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton_NC_CheckedChanged);
			// 
			// radioButton_C2
			// 
			this->radioButton_C2->AutoSize = true;
			this->radioButton_C2->Location = System::Drawing::Point(7, 45);
			this->radioButton_C2->Name = L"radioButton_C2";
			this->radioButton_C2->Size = System::Drawing::Size(53, 16);
			this->radioButton_C2->TabIndex = 1;
			this->radioButton_C2->TabStop = true;
			this->radioButton_C2->Text = L"Class2";
			this->radioButton_C2->UseVisualStyleBackColor = true;
			this->radioButton_C2->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton_C2_CheckedChanged);
			// 
			// radioButton_C1
			// 
			this->radioButton_C1->AutoSize = true;
			this->radioButton_C1->Location = System::Drawing::Point(7, 22);
			this->radioButton_C1->Name = L"radioButton_C1";
			this->radioButton_C1->Size = System::Drawing::Size(53, 16);
			this->radioButton_C1->TabIndex = 0;
			this->radioButton_C1->TabStop = true;
			this->radioButton_C1->Text = L"Class1";
			this->radioButton_C1->UseVisualStyleBackColor = true;
			this->radioButton_C1->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton_C1_CheckedChanged);
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->hScrollBar1);
			this->groupBox7->Controls->Add(this->groupBox8);
			this->groupBox7->Location = System::Drawing::Point(569, 288);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(139, 238);
			this->groupBox7->TabIndex = 18;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"Group Input";
			// 
			// hScrollBar1
			// 
			this->hScrollBar1->Location = System::Drawing::Point(8, 204);
			this->hScrollBar1->Name = L"hScrollBar1";
			this->hScrollBar1->Size = System::Drawing::Size(120, 17);
			this->hScrollBar1->TabIndex = 10;
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->radioButton8);
			this->groupBox8->Controls->Add(this->label5);
			this->groupBox8->Controls->Add(this->textBox8);
			this->groupBox8->Controls->Add(this->radioButton7);
			this->groupBox8->Controls->Add(this->label6);
			this->groupBox8->Controls->Add(this->label8);
			this->groupBox8->Controls->Add(this->Y);
			this->groupBox8->Controls->Add(this->textBox5);
			this->groupBox8->Controls->Add(this->X);
			this->groupBox8->Controls->Add(this->label7);
			this->groupBox8->Location = System::Drawing::Point(8, 18);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(125, 183);
			this->groupBox8->TabIndex = 0;
			this->groupBox8->TabStop = false;
			// 
			// radioButton8
			// 
			this->radioButton8->AutoSize = true;
			this->radioButton8->Location = System::Drawing::Point(17, 45);
			this->radioButton8->Name = L"radioButton8";
			this->radioButton8->Size = System::Drawing::Size(64, 16);
			this->radioButton8->TabIndex = 1;
			this->radioButton8->TabStop = true;
			this->radioButton8->Text = L"Gaussian";
			this->radioButton8->UseVisualStyleBackColor = true;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(4, 149);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(66, 12);
			this->label5->TabIndex = 8;
			this->label5->Text = L"Rotate Angle";
			// 
			// textBox8
			// 
			this->textBox8->Location = System::Drawing::Point(71, 139);
			this->textBox8->Name = L"textBox8";
			this->textBox8->Size = System::Drawing::Size(49, 22);
			this->textBox8->TabIndex = 9;
			// 
			// radioButton7
			// 
			this->radioButton7->AutoSize = true;
			this->radioButton7->Location = System::Drawing::Point(17, 21);
			this->radioButton7->Name = L"radioButton7";
			this->radioButton7->Size = System::Drawing::Size(63, 16);
			this->radioButton7->TabIndex = 0;
			this->radioButton7->TabStop = true;
			this->radioButton7->Text = L"Uniform";
			this->radioButton7->UseVisualStyleBackColor = true;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 74);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(48, 12);
			this->label6->TabIndex = 2;
			this->label6->Text = L"#of Point";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(4, 126);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(59, 12);
			this->label8->TabIndex = 4;
			this->label8->Text = L"Range of Y";
			// 
			// Y
			// 
			this->Y->Location = System::Drawing::Point(71, 116);
			this->Y->Name = L"Y";
			this->Y->Size = System::Drawing::Size(49, 22);
			this->Y->TabIndex = 7;
			// 
			// textBox5
			// 
			this->textBox5->Location = System::Drawing::Point(71, 64);
			this->textBox5->Name = L"textBox5";
			this->textBox5->Size = System::Drawing::Size(49, 22);
			this->textBox5->TabIndex = 5;
			// 
			// X
			// 
			this->X->Location = System::Drawing::Point(71, 88);
			this->X->Name = L"X";
			this->X->Size = System::Drawing::Size(49, 22);
			this->X->TabIndex = 6;
			this->X->TextChanged += gcnew System::EventHandler(this, &Form1::X_TextChanged);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(4, 98);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(59, 12);
			this->label7->TabIndex = 3;
			this->label7->Text = L"Range of X";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(569, 536);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(25, 12);
			this->label9->TabIndex = 19;
			this->label9->Text = L"File:";
			// 
			// textBox_Filename
			// 
			this->textBox_Filename->Location = System::Drawing::Point(597, 532);
			this->textBox_Filename->Name = L"textBox_Filename";
			this->textBox_Filename->Size = System::Drawing::Size(111, 22);
			this->textBox_Filename->TabIndex = 20;
			// 
			// richTextBox1
			// 
			this->richTextBox1->Location = System::Drawing::Point(569, 560);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->Size = System::Drawing::Size(141, 100);
			this->richTextBox1->TabIndex = 21;
			this->richTextBox1->Text = L"";
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// Classification
			// 
			this->Classification->Controls->Add(this->comboBox_classify);
			this->Classification->Location = System::Drawing::Point(719, 109);
			this->Classification->Name = L"Classification";
			this->Classification->Size = System::Drawing::Size(139, 43);
			this->Classification->TabIndex = 22;
			this->Classification->TabStop = false;
			this->Classification->Text = L"Classification";
			// 
			// comboBox_classify
			// 
			this->comboBox_classify->FormattingEnabled = true;
			this->comboBox_classify->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"Bayes-Map"});
			this->comboBox_classify->Location = System::Drawing::Point(7, 15);
			this->comboBox_classify->Name = L"comboBox_classify";
			this->comboBox_classify->Size = System::Drawing::Size(121, 20);
			this->comboBox_classify->TabIndex = 0;
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->label10);
			this->groupBox9->Controls->Add(this->textBox1);
			this->groupBox9->Controls->Add(this->checkBox_Unbiased);
			this->groupBox9->Location = System::Drawing::Point(719, 287);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(128, 80);
			this->groupBox9->TabIndex = 23;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"Bayes(Map)";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(7, 53);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(69, 12);
			this->label10->TabIndex = 2;
			this->label10->Text = L"Ellipse sigma:";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(82, 43);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(20, 22);
			this->textBox1->TabIndex = 1;
			// 
			// checkBox_Unbiased
			// 
			this->checkBox_Unbiased->AutoSize = true;
			this->checkBox_Unbiased->Location = System::Drawing::Point(7, 21);
			this->checkBox_Unbiased->Name = L"checkBox_Unbiased";
			this->checkBox_Unbiased->Size = System::Drawing::Size(67, 16);
			this->checkBox_Unbiased->TabIndex = 0;
			this->checkBox_Unbiased->Text = L"Unbiased";
			this->checkBox_Unbiased->UseVisualStyleBackColor = true;
			// 
			// clustering
			// 
			this->clustering->Controls->Add(this->comboBox_clusters);
			this->clustering->Controls->Add(this->comboBox_clustering);
			this->clustering->Location = System::Drawing::Point(719, 159);
			this->clustering->Name = L"clustering";
			this->clustering->Size = System::Drawing::Size(139, 74);
			this->clustering->TabIndex = 24;
			this->clustering->TabStop = false;
			this->clustering->Text = L"clustering";
			// 
			// comboBox_clusters
			// 
			this->comboBox_clusters->FormattingEnabled = true;
			this->comboBox_clusters->Location = System::Drawing::Point(49, 48);
			this->comboBox_clusters->Name = L"comboBox_clusters";
			this->comboBox_clusters->Size = System::Drawing::Size(84, 20);
			this->comboBox_clusters->TabIndex = 1;
			// 
			// comboBox_clustering
			// 
			this->comboBox_clustering->FormattingEnabled = true;
			this->comboBox_clustering->Location = System::Drawing::Point(49, 21);
			this->comboBox_clustering->Name = L"comboBox_clustering";
			this->comboBox_clustering->Size = System::Drawing::Size(84, 20);
			this->comboBox_clustering->TabIndex = 0;
			// 
			// regression
			// 
			this->regression->Controls->Add(this->comboBox_NL_degree);
			this->regression->Controls->Add(this->comboBox_regression);
			this->regression->Location = System::Drawing::Point(719, 240);
			this->regression->Name = L"regression";
			this->regression->Size = System::Drawing::Size(139, 42);
			this->regression->TabIndex = 25;
			this->regression->TabStop = false;
			this->regression->Text = L"regression";
			// 
			// comboBox_NL_degree
			// 
			this->comboBox_NL_degree->FormattingEnabled = true;
			this->comboBox_NL_degree->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"2", L"3", L"4", L"5", L"6", L"7", L"8", 
				L"9", L"10"});
			this->comboBox_NL_degree->Location = System::Drawing::Point(82, 16);
			this->comboBox_NL_degree->Name = L"comboBox_NL_degree";
			this->comboBox_NL_degree->Size = System::Drawing::Size(46, 20);
			this->comboBox_NL_degree->TabIndex = 1;
			this->comboBox_NL_degree->Text = L"2";
			// 
			// comboBox_regression
			// 
			this->comboBox_regression->FormattingEnabled = true;
			this->comboBox_regression->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"Linear", L"Linear-Ln", L"1", L"1", 
				L"Nonlinear"});
			this->comboBox_regression->Location = System::Drawing::Point(9, 16);
			this->comboBox_regression->Name = L"comboBox_regression";
			this->comboBox_regression->Size = System::Drawing::Size(65, 20);
			this->comboBox_regression->TabIndex = 0;
			this->comboBox_regression->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox_regression_SelectedIndexChanged);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(864, 689);
			this->Controls->Add(this->regression);
			this->Controls->Add(this->clustering);
			this->Controls->Add(this->groupBox9);
			this->Controls->Add(this->Classification);
			this->Controls->Add(this->richTextBox1);
			this->Controls->Add(this->textBox_Filename);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->groupBox7);
			this->Controls->Add(this->groupBox6);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->Run);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button_Clear);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->textBox_Y);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->textBox_X);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"Machine Learning";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->Run->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->groupBox7->ResumeLayout(false);
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->Classification->ResumeLayout(false);
			this->groupBox9->ResumeLayout(false);
			this->groupBox9->PerformLayout();
			this->clustering->ResumeLayout(false);
			this->regression->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void fileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
					myBitmap = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height, PixelFormat::Format24bppRgb);
					g = Graphics::FromImage(myBitmap);
					button_Clear_Click(sender, e);
					Pi = 4.0 * atan(1.0);
					imW = pictureBox1->Width;
					imH = pictureBox1->Height;
					CenterX=0.5*imW; //Center X®y¼Ð
					CenterY=0.5*imH; //Center Y®y¼Ð
					comboBox_psize->SelectedIndex=1; //PointSize=2¡A¹w³]¸ê®ÆÂI¤j¤p¡C
					PointSize=(comboBox_psize->SelectedIndex+1)*5;
					PointSize1=PointSize+2; //¸ê®ÆÂI¹Bºâ§¹µ²ªG¤j¤p
					PointSize2=PointSize+4; //Class or Cluster ¤¤¤ß¤j¤p
					radioButton_C1->Checked=true; //ClassKind=1; Color=Red
					ClassKind=1; //ClassKind=1; Color=Red, ClassKind=-1; Color=Blue
					//comboBox_CS Items initializing, i.e. Class(Target) selection.
					for (int i=0;i<10;i++)
					comboBox_CS->Items->Add(Convert::ToString(i));
					comboBox_CS->SelectedIndex=0;
					comboBox_CS->Enabled=false;	
					HandFlag=true; //HandFlag=true®É¡APictureBox1_MouseClick¥i¿é¤JData Points¡C¤Ï¤§¡A«h¤£¯à¡C
					//Run Program
					comboBox_Run->SelectedIndex=0; //Run Classification-- 1:Clustering-- 2:Regression
					comboBox_clustering->Enabled=false; //Disable Clustering
					comboBox_clusters->Enabled=false; //Disable Clustering
					comboBox_regression->Enabled=false; //Disable Regression
					//classification Method
					comboBox_classify->SelectedIndex=0;
					MaxSizeOfData=Convert::ToInt32(textBox_MaxSize->Text); //MaxSize Of Input Data
					//InputData = new pData[MaxSizeOfData];
					NewPublicVariables(MaxSizeOfData);
					comboBox_NL_degree->Enabled=false; //Nonlinear Regression only use.

			 }
			private: System::Void pictureBox1_Move(System::Object^ sender,System::Windows::Forms::MouseEventArgs^ e) {
					//textBox_X->Text =Convert::ToString((e->X-256.0)/256.0);
					//textBox_Y->Text =Convert::ToString((256.0-e->Y)/256.0);
					textBox_X->Text =Convert::ToString((e->X-CenterX)/CenterX);
					textBox_Y->Text =Convert::ToString((CenterY-e->Y)/CenterY);
			}
			private: System::Void pictureBox1_Down(System::Object^ sender,System::Windows::Forms::MouseEventArgs^ e) {
					X_Cur=e->X;
					Y_Cur=e->Y;
			}

			private: System::Void button_Clear_Click(System::Object^  sender, System::EventArgs^  e) {
					 clearToolStripMenuItem_Click(sender,e);
					 NumberOfData=0;
					 textBox_datasize->Text = "0";
					 Filename1="";
					 textBox_Filename->Text ="";
					 richTextBox1->Clear();

			}
		    private: System::Void pictureBox1_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) { 
					double X_tmp=0.0,Y_tmp=0.0;

					if (HandFlag) { //HandFlag=true®É¡APictureBox1_MouseClick¥i¿é¤JData Points¡C¤Ï¤§¡A«h¤£¯à¡C
						if (radioButton_Single->Checked) {
							//X_tmp=(double)(X_Cur-256.0)/256.0;
							//Y_tmp=(double)(256.0-Y_Cur)/256.0;
							X_tmp=(double)(X_Cur-CenterX)/CenterX;
							Y_tmp=(double)(CenterY-Y_Cur)/CenterY;
							InputData[NumberOfData].X=X_tmp;
							InputData[NumberOfData].Y=Y_tmp;

						if (radioButton_C1->Checked){
							InputData[NumberOfData].ClassKind=1; //Red Color
							NumClass1++;
						}//if (radioButton_C1->Checked)
						else if (radioButton_C2->Checked){
							InputData[NumberOfData].ClassKind=-1; //Blue Color
							NumClass2++;
						}//else if (radioButton_C2->Checked)
						else if (radioButton_NC->Checked){
							InputData[NumberOfData].ClassKind=-2; //Black Color
						}//else if (radioButton_NC->Checked)
						else{
							InputData[NumberOfData].ClassKind=comboBox_CS->SelectedIndex; //Select Class Color
						}//else
							bshDraw=ClassToColor(InputData[NumberOfData].ClassKind);
							g->FillEllipse(bshDraw, X_Cur-PointSize/2, Y_Cur-PointSize/2, PointSize, PointSize);
							NumberOfData++;
						textBox_datasize->Text = Convert::ToString(NumberOfData);
					}//if single input
						else { //group input
							MessageBox::Show("«Øºc¤¤¡A½Ð­@¤ßµ¥­Ô¡C");
						}//else Group input
					}//if (HandFlag)
					pictureBox1->Image = myBitmap;
					pictureBox1->Refresh();
						//pictureBox1.MouseMove += gcnew MouseEventHandler(pictureBox1_Move);
						//pictureBox1.MouseDown += gcnew MouseEventHandler(pictureBox1_Down);
	     }

private: System::Void clearToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					 g->Clear(Color::White); //Paint over the image area in white.
					//bshDraw = gcnew SolidBrush(Color::White); //²M°£µe¥¬¥t¤@ºØ¤èªk
					//g->FillRectangle(bshDraw, 0, 0, 512, 512);
					//Draw
					penDraw = gcnew Pen(Color::Black, 1); //µeµ§¬O¶Â¦âªº, µe®Ø
					//g->DrawLine(penDraw, 0, 0, 0, 511);
					//g->DrawLine(penDraw, 0, 0, 511, 0);
					//g->DrawLine(penDraw, 0, 511, 511, 511);
					//g->DrawLine(penDraw, 511, 0, 511, 511);
					g->DrawLine(penDraw, 0, 0, 0, pictureBox1->Height-1);
					g->DrawLine(penDraw, 0, 0, pictureBox1->Width-1, 0);
					g->DrawLine(penDraw, 0, pictureBox1->Height-1, pictureBox1->Width-1, pictureBox1->Height-1);
					g->DrawLine(penDraw, pictureBox1->Width-1, 0, pictureBox1->Width-1, pictureBox1->Height-1);
					pictureBox1->Image = myBitmap;
					pictureBox1->Refresh();
		 }
private: System::Void X_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void saveToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					if (NumberOfData>0) {
						richTextBox1->Clear();
						for (int i=0; i<NumberOfData; i++) {
						 String^ line = Convert::ToString(InputData[i].X) + "\t" + Convert::ToString(InputData[i].Y) + "\t"+
						 Convert::ToString(InputData[i].ClassKind)+ "\n";
						 richTextBox1->AppendText(line);
						}//for i
					saveFileDialog1->Filter="*.TXT|*.txt|*.DAT|*.dat|All Files|*.*";
					//saveFileDialog1->DefaultExt="dat";
					saveFileDialog1->DefaultExt = "txt";
					if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK 
						&&saveFileDialog1->FileName->Length >0) {
						 richTextBox1->SaveFile(saveFileDialog1->FileName);
						 Filename1 = saveFileDialog1->FileName;
						 textBox_Filename->Text = saveFileDialog1->FileName;
						}//if saveFileDialog1
				    }//if (NumberOfData>0)

		 }
private: System::Void toolStripButton2_Click(System::Object^  sender, System::EventArgs^  e) {
					if ( String::IsNullOrEmpty(Filename1) )
					saveToolStripMenuItem_Click(sender, e);
				    else
					richTextBox1->SaveFile(Filename1);

		 }
private: System::Void openToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					 bool openfile = true;
					 if (richTextBox1->Modified) { //§PÂ_ richTextBox1¬O§_¦³¸g¹L¥ô¦ó½s¿è
						bool openfile = false;
						if (MessageBox::Show("¥¼¦sÀÉ!¬O§_Ä~Äò", "½T»{µøµ¡", MessageBoxButtons::YesNo,
						MessageBoxIcon::Question)==System::Windows::Forms::DialogResult::Yes)
						openfile = true;
					 }//if (richTextBox1->Modified)
					 if (openfile) {
						openFileDialog1->Filter="*.TXT|*.txt|*.DAT|*.dat|All Files|*.*";
					 if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
						 button_Clear_Click(sender, e);
						 richTextBox1->LoadFile(openFileDialog1->FileName);
						 Filename1 = openFileDialog1->FileName;
						 textBox_Filename->Text = openFileDialog1->SafeFileName;
						 NumberOfData=0;
						 //char* Token =(char*)(void*)Marshal::StringToHGlobalAnsi(richTextBox1->Text); 
						 //char* Token =(char*)(void*)Marshal::StringToHGlobalAnsi(richTextBox1->Text);
						 char* Token=(char*)Marshal::StringToHGlobalAnsi(richTextBox1->Text).ToPointer();
						 char* sptoken = strtok(Token," \t\n");
						 while (sptoken != NULL) {
						 InputData[NumberOfData].X= atof(sptoken);
						 sptoken = strtok(NULL," \t\n");
						 InputData[NumberOfData].Y= atof(sptoken);
						 sptoken = strtok(NULL," \t\n");
						 InputData[NumberOfData].ClassKind= atoi(sptoken);
						 sptoken = strtok(NULL," \t\n");
						 NumberOfData++;
						 }//while
						 textBox_datasize->Text = Convert::ToString(NumberOfData);
						 showToolStripMenuItem_Click(sender, e); // // Show data		 
						}
					 }
		 }
private: System::Void showToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
								clearToolStripMenuItem_Click(sender, e);
					 //DrawData
					 for (int i=0; i<NumberOfData; i++) {
					 //X_Cur=(int)(InputData[i].X*256+256);
					 //Y_Cur=(int)(256-InputData[i].Y*256);
					 X_Cur=(int)(InputData[i].X*CenterX+CenterX);
					 Y_Cur=(int)(CenterY-InputData[i].Y*CenterY);
					 bshDraw=ClassToColor(InputData[i].ClassKind);
					 g->FillEllipse(bshDraw, X_Cur, Y_Cur, PointSize, PointSize);
					 }// for
					 pictureBox1->Image = myBitmap;
					 pictureBox1->Refresh();


		 }
private: System::Void radioButton_C1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
					 if (radioButton_C1->Checked){
						ClassKind=1; //ClassKind=1; Color=Red, ClassKind=-1; Color=Blue
					 }//if
		 }
private: System::Void radioButton_C2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
					 if (radioButton_C2->Checked){
						 ClassKind=-1; //ClassKind=1; Color=Red, ClassKind=-1; Color=Blue
					 }//if
		 }
private: System::Void radioButton_NC_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
					 if (radioButton_NC->Checked){
						ClassKind=-2; //ClassKind=-2; Color=Black
					 }//if
		 }
private: System::Void radioButton_S1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
					 comboBox_CS->Enabled=radioButton_S1->Checked;
					 if (radioButton_S1->Checked){
						 ClassKind=comboBox_CS->SelectedIndex;
					 }//if
		 }
private: System::Void comboBox_CS_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
					 if (radioButton_S1->Checked){
					     ClassKind=comboBox_CS->SelectedIndex;
					 }//if
		 }
private: System::Void comboBox_psize_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
					 PointSize=(comboBox_psize->SelectedIndex+1)*5;
					 PointSize1=PointSize+2;
					 PointSize2=PointSize+4;
		 }
private: System::Void textBox_MaxSize_TextChanged(System::Object^  sender, System::EventArgs^  e) {
					 MaxSizeOfData=Convert::ToInt32(textBox_MaxSize->Text);
					 if (MaxSizeOfData<1000){
						MaxSizeOfData=1000;
						textBox_MaxSize->Text=Convert::ToString(MaxSizeOfData);
					 }//if
					 else {
						//DeletePublicVariables(MaxSizeOfData);
						delete [] InputData;
						//NewPublicVariables(MaxSizeOfData);
						InputData = new pData[MaxSizeOfData];
					 }//else
					 clearToolStripMenuItem_Click(sender, e);
		 }
private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					 delete g;
					 delete [] InputData;
					 DeletePublicVariables(MaxSizeOfData);
					 Application::Exit();
		 }
private: System::Void toolStripButton3_Click(System::Object^  sender, System::EventArgs^  e) {
					exitToolStripMenuItem_Click(sender,e);
		 }


private: System::Void showDataToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					 clearToolStripMenuItem_Click(sender, e);
					 //DrawData
					 for (int i=0; i<NumberOfData; i++) {
						 //X_Cur=(int)(InputData[i].X*256+256);
						 //Y_Cur=(int)(256-InputData[i].Y*256);
						 X_Cur=(int)(InputData[i].X*CenterX+CenterX);
						 Y_Cur=(int)(CenterY-InputData[i].Y*CenterY);
						 bshDraw=ClassToColor(InputData[i].ClassKind);
						 g->FillEllipse(bshDraw, X_Cur, Y_Cur, PointSize, PointSize);
					 }// for
					 pictureBox1->Image = myBitmap;
					 pictureBox1->Refresh();
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
					MethodCodeValue = 0;
					int methodbased = 20;
				switch (comboBox_Run->SelectedIndex) {
					case 0: //Classification
					MethodCodeValue = (comboBox_Run->SelectedIndex)*methodbased + comboBox_classify->SelectedIndex;
					break;
					case 1: //Clustering
					MethodCodeValue = (comboBox_Run->SelectedIndex)*methodbased + comboBox_clustering->SelectedIndex;
					break;
					case 2: //Regression
					MethodCodeValue = (comboBox_Run->SelectedIndex)*methodbased + comboBox_regression->SelectedIndex;
					break;
					default:
					MessageBox::Show("µL«Øºc¦¹Ãþ§O¤èªk!");
				}//switch --- Run Method-----Based = 20
				switch (MethodCodeValue) {
					case 0: //Classification--Bayes-MAP
					BayesMAP();
					showContourToolStripMenuItem_Click(sender, e);
					showResultToolStripMenuItem_Click(sender, e);
					showMeansToolStripMenuItem_Click(sender, e);
					break;
					case 1: //k-NN
					break;
					case 40: //Regression--Linear
					LinearRegression();
					showDataToolStripMenuItem_Click(sender, e);
					showRegressionToolStripMenuItem_Click(sender, e);
					break;
					case 41: //Regression--Linear-ln() == log e
					LinearRegressionLn();
					showDataToolStripMenuItem_Click(sender, e);
					showRegressionToolStripMenuItem_Click(sender, e);
					break;
					case 44: //Nonlinear Regression--Degree == NLdegree
					NLdegree= comboBox_NL_degree->SelectedIndex+2;
					A=new double*[NLdegree+1];
					for (int i=0; i<NLdegree+1; i++)
					A[i]= new double[NLdegree+1];
					B=new double[NLdegree+1];
					NLcoef=new double[NLdegree+1];
					NonlinearRegression(NLdegree);
					showDataToolStripMenuItem_Click(sender, e);
					showRegressionToolStripMenuItem_Click(sender, e);
					//delete
					//delete
					for (int i=0; i<NLdegree+1; i++)
					delete [] A[i];
					delete [] A;
					delete [] B;
					delete [] NLcoef;
					break;
					default:
					MessageBox::Show("µL«Øºc¦¹Ãþ§O¤èªk!");
					}//switch (MethodCodeValue)
		 }



private: System::Void showResultToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					double Output_yi;
					Brush^ errBrush = gcnew SolidBrush(Color::DeepPink); //Classified error color
					//Draw Data
					for (int i = 0; i<NumberOfData; i++) {
					X_Cur = (int)(InputData[i].X*CenterX + CenterX); //CenterX=256.0
					Y_Cur = (int)(CenterY - InputData[i].Y*CenterY); //CenterY=256.0
					switch (MethodCodeValue) {
					case 0: //Bayesian-MAP
					Output_yi = PClass1*PxyClass1[i] - PClass2*PxyClass2[i];
					if (Sgn((double)InputData[i].ClassKind) == Sgn(Output_yi)) {
					bshDraw = ClassToColor(InputData[i].ClassKind);
					g->FillEllipse(bshDraw, X_Cur - PointSize1 / 2, Y_Cur - PointSize1 / 2, PointSize1, PointSize1);
					}//if (Sgn((double) InputData[i].ClassKind)== Sgn(Output_yi))
					else {
					g->FillEllipse(errBrush, X_Cur - PointSize1 / 2, Y_Cur - PointSize1 / 2, PointSize1, PointSize1);
					}//else
					break;	
					case 1: //K-NN
					break;
					default:
					MessageBox::Show("µL«Øºc¦¹Ãþ§O¤èªk!");
					}//switch
					}//for
					pictureBox1->Image = myBitmap;
					pictureBox1->Refresh();
		 }
private: System::Void showContourToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					double Output_yi;
					pData Sample;
					unsigned char blockcolor=0, LightLevel, ColorScale;
					int Lighttmp;
					//DrawData
					// Lock the bitmap's bits.
					Rectangle rect = Rectangle(0, 0, imW, imH);
					BitmapData^ bmpData = myBitmap->LockBits(rect, ImageLockMode::ReadWrite, myBitmap->PixelFormat);
					int ByteOfSkip = bmpData->Stride - bmpData->Width * 3;//­pºâ¨C¦æ«á­±´X­Ó Padding bytes , ¥þ±m¼v¹³
					unsigned char* p = (unsigned char*)bmpData->Scan0.ToPointer();
					int index = 0;
					for (int y = 0; y < imH; y++){
					for (int x = 0; x < imW; x++){
					Sample.X = (double)(x - CenterX) / CenterX;
					Sample.Y = (double)(CenterY - y) / CenterY;
					switch (MethodCodeValue) {
					case 0: //Bayesian-MAP
					ColorScale = 200;
					Output_yi = PClass1*evalPxy1(Sample) - PClass2*evalPxy2(Sample);
					break;
					case 1: //K-NN
					break;
					default:
					MessageBox::Show("µL«Øºc¦¹Ãþ§O¤èªk!");
					}//switch
					//Show Contour Type
					LightLevel = 155; //Fixed or Soft „³ LightLevel =Min(155, (int)abs(255.0* Output_yi * ColorScale) );
					if (Output_yi>0.0) {
					p[index + 0] = LightLevel; //Red
					p[index + 1] = LightLevel; //Green
					p[index + 2] = 255 - blockcolor; //Blue
					}//if
					else if (Output_yi == 0.0) {
					p[index + 0] = 255; //Red
					p[index + 1] = 255; //Green
					p[index + 2] = 255; //Blue
					}//else if (Output_yi==0)
					else {
					p[index + 0] = 255 - blockcolor; //Red
					p[index + 1] = LightLevel; //Green
					p[index + 2] = LightLevel; //Blue
					}//else
					index += 3;
					}//for x
					index += ByteOfSkip; // ¸õ¹L³Ñ¤Uªº Padding bytes
					}//for y
					// Unlock the bits.
					myBitmap->UnlockBits(bmpData);
					pictureBox1->Image = myBitmap;
					pictureBox1->Refresh();
		 }
private: System::Void showMeansToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
					X_Cur = (int)(MeanX1*CenterX + CenterX); //CenterX=256.0
					Y_Cur = (int)(CenterY - MeanY1*CenterY); //CenterY=256.0
					//bshDraw=ClassToColor(1); //Red
					//g->FillEllipse(bshDraw, X_Cur-PointSize2/2, Y_Cur-PointSize2/2, PointSize2, PointSize2);
					penDraw = ClassToPenColor(1); //Red
					g->DrawEllipse(penDraw, X_Cur - PointSize2 / 2, Y_Cur - PointSize2 / 2, PointSize2, PointSize2);
					//Draw Means 2
					X_Cur = (int)(MeanX2*CenterX + CenterX);
					Y_Cur = (int)(CenterY - MeanY2*CenterY);
					//bshDraw=ClassToColor(-1); //Blue
					//g->FillEllipse(bshDraw, X_Cur-PointSize2/2, Y_Cur-PointSize2/2, PointSize2, PointSize2);
					penDraw = ClassToPenColor(-1); //Blue
					g->DrawEllipse(penDraw, X_Cur - PointSize2 / 2, Y_Cur - PointSize2 / 2, PointSize2, PointSize2);
					pictureBox1->Image = myBitmap;
					pictureBox1->Refresh();

		 }
private: System::Void comboBox_Run_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
					switch (comboBox_Run->SelectedIndex) {
					case 0: //Classification
					comboBox_classify->Enabled=true; //Enable Classification
					comboBox_clustering->Enabled=false; //Disable Clustering
					comboBox_clusters->Enabled=false; //Disable Clustering
					comboBox_regression->Enabled=false; //Disable Regression
					break;
					case 1: //Clustering
				
					case 2: //Regression
					
					comboBox_regression->Enabled=true; // Enable Regression
					break;
					default: 
					comboBox_classify->Enabled=true; //Enable Classification
					comboBox_clustering->Enabled=false; //Disable Clustering
					comboBox_clusters->Enabled=false; //Disable Clustering
					comboBox_regression->Enabled=false; //Disable Regression
					}//switch

		 }
private: System::Void comboBox_regression_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
					showDataToolStripMenuItem_Click(sender, e);
					if (comboBox_regression->SelectedIndex==4)
					comboBox_NL_degree->Enabled=true; //Nonlinear Regression only use.
					else
					comboBox_NL_degree->Enabled=false; //Nonlinear Regression only use.
		 }
private: System::Void showRegressionToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 double LowBound, HighBound, tmpX, tmpY, wk, sumW, s2;
			int X0, Y0;
			LowBound=-1.0; HighBound=1.0;
			X0=0; //tmpX=-1.0 , i.e. shiftX=1.0
			switch (comboBox_regression->SelectedIndex) {
			case 0: //Linear
			tmpY = LR_a0 - LR_a1; //tmpX=-1.0
			break;
			case 1: //Linear--Ln
			tmpY = LR_a0*exp(LR_a1); //tmpX=-1.0 , i.e. shiftX=1.0, ==>Y = a0 * exp^(a1*1.0)
			LowBound=1.0; HighBound=3.0; //CenterX=256.0 (==2.0) ==>[1.0,3.0]
			break;
			//case 2: //Linear¡XLog10

			//break;
			case 4: //Nonlinear
			//tmpY =NLcoef[0]-NLcoef[1];
			//for (int i=2; i<NLdegree+1;i++)
			// tmpY += NLcoef[i]*pow(-1.0,i); //tmpX=-1.0
			tmpY =NLcoef[NLdegree];
			for (int i=NLdegree-1; i>=0;i--)
			tmpY = -tmpY+NLcoef[i]; //tmpX=-1.0
			break;
			default:
			tmpY = LR_a0 - LR_a1; //tmpX=-1.0
			}//switch
			Y0=(int)((HighBound-tmpY)*CenterY);
			while (Y0<1 || Y0 >imH-2 ) {
			X0++;
			switch (comboBox_regression->SelectedIndex) {
			case 0: //Linear
			tmpX = (double) (X0-CenterX)/CenterX;
			tmpY = LR_a0 + LR_a1*tmpX;
			break;
			case 1: //Linear--Ln
			tmpX = (double) (X0+CenterX)/CenterX;
			tmpY = LR_a0*exp(LR_a1*tmpX);
			LowBound=1.0; HighBound=3.0; //CenterX=256.0 (==2.0) ==>[1.0,3.0]
			break;
			case 4: //Nonlinear
			tmpX = (double) (X0-CenterX)/CenterX; //tmpX=[-1.0,1.0]

			tmpY =NLcoef[NLdegree];
			for (int i=NLdegree-1; i>=0;i--)
			tmpY = tmpY*tmpX+NLcoef[i];
			break;
			default:
			tmpY = LR_a0 + LR_a1*tmpX;
			}//switch
			Y0=(int)((HighBound-tmpY)*CenterY);
			}//while
			for (int x = X0+1; x < imW-1; x++){ 
				switch (comboBox_regression->SelectedIndex) {
				case 0: //Linear
				tmpX= (double) (x-CenterX)/CenterX;
				tmpY = LR_a0 + LR_a1 * tmpX; //Y = a0 + a1*X
				break;
				case 1: //Linear--Ln
				tmpX = (double) (x+CenterX)/CenterX;
				tmpY = LR_a0*exp(LR_a1*tmpX); //Y = a0 * exp^(a1*X)
				LowBound=1.0; HighBound=3.0; //CenterX=256.0 (==2.0) ==>[1.0,3.0]
				break;
				case 4: //Nonlinear
				tmpX= (double) (x-CenterX)/CenterX; //tmpX=[-1.0,1.0]
				//tmpY =NLcoef[0];
				//for (int i=1; i<NLdegree+1;i++)
				//tmpY += NLcoef[i]*pow(tmpX,i);
				tmpY =NLcoef[NLdegree];
				for (int i=NLdegree-1; i>=0;i--)
				tmpY = tmpY*tmpX+NLcoef[i];
				break;
				default: //Linear
				tmpX= (double) (x-CenterX)/CenterX;
				tmpY = LR_a0 + LR_a1 * tmpX; //Y = a0 + a1*X
				}//switch
				if (tmpY > LowBound && tmpY < HighBound ) {
					X_Cur=x;
					Y_Cur=(int)((HighBound-tmpY)*CenterY); //CenterY=256.0
					penDraw=ClassToPenColor(-1); //Blue
					g->DrawLine(penDraw, X0, Y0, X_Cur, Y_Cur);
					X0=X_Cur;
					Y0=Y_Cur;
					}//if
					}//for
					pictureBox1->Image = myBitmap;
					pictureBox1->Refresh();
			
		 }
	void LinearRegression(){
			double allx;double ally;double allx2;double allxy;
			allx=0;ally=0;allx2=0;allxy=0;
			 for (int i = 0; i < NumberOfData; i++){
				 double xy=InputData[i].X*InputData[i].X;
					allx += InputData[i].X;
					ally += InputData[i].Y;
					allx2 += InputData[i].X*InputData[i].X;
					allxy += InputData[i].X*InputData[i].Y;
			 }
			 LR_a1 = (NumberOfData*allxy-allx*ally)/(NumberOfData*allx2 -allx*allx);
			 LR_a0 = ally/NumberOfData- LR_a1*allx/NumberOfData;
			 
		} 
	void LinearRegressionLn(){
				
				double allx;double ally;double allx2;double allxy;double sx;double sy;double lny;
			    allx=0;ally=0;allx2=0;allxy=0;sx=0;sy=0;lny=0;

			      for (int i = 0; i < NumberOfData; i++){
					  sx=InputData[i].X+2.0;
					  sy=InputData[i].Y+2.0;

					  lny = log(sy);
					  double xy=sx*sx;
					  allx += sx;
					  ally += lny;
					  allx2 += sx*sx;
					  allxy += sx*lny;

				  }

			 LR_a1 = (NumberOfData*allxy-allx*ally)/(NumberOfData*allx2 -allx*allx);
			 LR_a0 = ally/NumberOfData- LR_a1*allx/NumberOfData;
			 LR_a0 =exp(LR_a0);
	} 

	void GaussEliminationPivot(int n){
			double tmp, pvt;
			int index_pvt, *pivot;

			pivot = new int[n];

			for (int j = 0; j < n-1; j++) {
				pvt = abs(A[j][j]);
				pivot[j] = j;
				index_pvt = j;
				//find pivot
				for (int i = j+1; i < n; i++) {
					if (abs(A[i][j]) > pvt) {
						pvt = abs(A[i][j]);
						index_pvt = i;
					}//if
				}//for i

				//switch row pivot[j] and row index_pvt
				if (pivot[j] != index_pvt) {
					for (int i = 0; i < n; i++) {
						tmp = A[pivot[j]][i];
						A[pivot[j]][i] = A[index_pvt][i];
						A[index_pvt][i] = tmp;
					}//for i
					tmp = B[pivot[j]];
					B[pivot[j]] = B[index_pvt];
					B[index_pvt] = tmp;
				}//if

				for (int i = j+1; i < n; i++)
					A[i][j] /= A[j][j];

				//produce Upper triangle matrix
				for (int i = j+1; i < n; i++) {
					for (int k = j+1; k < n; k++) {
						A[i][k] -= A[i][j]*A[j][k];
					}//for k
					B[i] -= A[i][j] * B[j];
				}//for i
			}//for j

			//back substitution
			//for (int i = 0; i < n; i++)
			//	NLcoef[i] =0.0;
			NLcoef[n-1] = B[n-1] / A[n-1][n-1];
			for (int j = n-2; j >= 0; j--) {
				NLcoef[j] = B[j];
				for (int k = n-1; k > j; k--) {
					NLcoef[j] -= NLcoef[k]*A[j][k];
				}//for k
				NLcoef[j] /= A[j][j];
			}//for j
			delete [] pivot;
	}
	void NonlinearRegression(int degree){
		int u=0;
	double allx;double ally;double allx2;double allxy;double sx;double sy;double lny;
			    allx=0;ally=0;allx2=0;allxy=0;sx=0;sy=0;lny=0;int o=0;
			for(int i=0;i<3;i++){
					for(int j=0;j<3;j++){
						A[i][j]=0;}}
			for(int j=0;j<3;j++){
						B[j]=0;}

			for(int y=0;y<degree+1;y++){
				for(int x=0;x<degree+1;x++){
					for(int i=0;i<NumberOfData;i++){				
						A[y][x]+=pow(InputData[i].X,x+y);}
			}}

			for(int y=0;y<degree+1;y++){
				for(int x=0;x<NumberOfData;x++){
					
					B[y]+=pow(InputData[x].X,y)*InputData[x].Y;
			}}


		GaussEliminationPivot(degree+1);
	}

};
}

