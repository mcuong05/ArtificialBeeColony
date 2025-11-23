#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <time.h>

/* Tham số điều khiển của thuật toán ABC */
#define NP 40 /* Kích thước quần thể (tổng số ong làm việc + ong quan sát) */
#define FoodNumber NP/2 /* Số lượng nguồn thức ăn = một nửa kích thước quần thể */
#define limit 100  /* Một nguồn thức ăn không cải thiện sau "limit" lần thử sẽ bị bỏ bởi ong làm việc */
#define maxCycle 3000 /* Số chu kỳ lặp khi tìm kiếm (điều kiện dừng) */

/* Biến của bài toán cụ thể */
#define D 50 /* Số lượng tham số cần tối ưu */
#define lb -5.12 /* Giới hạn dưới của tham số */
#define ub 5.12 /* Giới hạn trên của tham số */

/* Số lần chạy để kiểm tra độ ổn định / độ bền vững của thuật toán */
#define runtime 30

double Foods[FoodNumber][D]; /* Ma trận nguồn thức ăn: mỗi hàng là một nghiệm D chiều */
double f[FoodNumber];  /* Giá trị hàm mục tiêu của từng nguồn */
double fitness[FoodNumber]; /* Giá trị fitness tương ứng của từng nguồn */
double trial[FoodNumber]; /* Số lần thử liên tiếp mà nghiệm không cải thiện */
double prob[FoodNumber]; /* Xác suất để chọn nguồn thức ăn */
double solution [D]; /* Nghiệm mới được tạo từ công thức v_{ij}=... */
double ObjValSol; /* Giá trị hàm mục tiêu của nghiệm mới */
double FitnessSol; /* Fitness của nghiệm mới */
int neighbour, param2change; /* neighbour: chỉ số nghiệm k; param2change: tham số j được thay đổi */
double GlobalMin; /* Giá trị tối ưu tốt nhất tìm được */
double GlobalParams[D]; /* Bộ tham số ứng với nghiệm tối ưu nhất */
double GlobalMins[runtime]; /* GlobalMin của mỗi lần chạy */
double r; /* Số ngẫu nhiên trong [0,1) */

/* con trỏ hàm tính hàm mục tiêu */
typedef double (*FunctionCallback)(double sol[D]);

/* Các hàm benchmark */
double sphere(double sol[D]);
double Rosenbrock(double sol[D]);
double Griewank(double sol[D]);
double Rastrigin(double sol[D]);

/* Chọn hàm mục tiêu */
FunctionCallback function = &Rastrigin;

/* Hàm tính fitness */
double CalculateFitness(double fun)
{
    double result=0;
    if(fun>=0)
    {
        result=1/(fun+1);
    }
    else
    {
        result=1+fabs(fun);
    }
    return result;
}

/* Ghi nhớ nghiệm tốt nhất hiện tại */
void MemorizeBestSource()
{
   int i,j;

   for(i=0;i<FoodNumber;i++)
   {
       if (f[i]<GlobalMin)
       {
           GlobalMin=f[i];
           for(j=0;j<D;j++)
               GlobalParams[j]=Foods[i][j];
       }
   }
}

/* Khởi tạo nghiệm ngẫu nhiên trong [lb, ub], đồng thời đặt lại trial */
void init(int index)
{
    int j;
    for (j=0;j<D;j++)
    {
        r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
        Foods[index][j]=r*(ub-lb)+lb;
        solution[j]=Foods[index][j];
    }
    f[index]=function(solution);
    fitness[index]=CalculateFitness(f[index]);
    trial[index]=0;
}

/* Khởi tạo toàn bộ nguồn thức ăn */
void initial()
{
    int i;
    for(i=0;i<FoodNumber;i++)
    {
        init(i);
    }
    GlobalMin=f[0];
    for(i=0;i<D;i++)
        GlobalParams[i]=Foods[0][i];
}

/* Pha ong làm việc: mỗi ong tạo nghiệm mới từ nghiệm của mình */
void SendEmployedBees()
{
  int i,j;

  for (i=0;i<FoodNumber;i++)
  {
      /* Chọn ngẫu nhiên tham số cần thay đổi */
      r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
      param2change=(int)(r*D);

      /* Chọn ngẫu nhiên nghiệm khác để tạo nghiệm mới */
      r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
      neighbour=(int)(r*FoodNumber);

      while(neighbour==i)
      {
          r=((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
          neighbour=(int)(r*FoodNumber);
      }

      for(j=0;j<D;j++)
          solution[j]=Foods[i][j];

      /* Công thức tạo nghiệm mới */
      r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
      solution[param2change] = Foods[i][param2change] +
                               (Foods[i][param2change] - Foods[neighbour][param2change])*(r-0.5)*2;

      /* Nếu vượt biên thì đưa về biên */
      if (solution[param2change]<lb) solution[param2change]=lb;
      if (solution[param2change]>ub) solution[param2change]=ub;

      ObjValSol=function(solution);
      FitnessSol=CalculateFitness(ObjValSol);

      /* Chọn nghiệm tốt hơn */
      if (FitnessSol>fitness[i])
      {
          trial[i]=0;
          for(j=0;j<D;j++)
              Foods[i][j]=solution[j];
          f[i]=ObjValSol;
          fitness[i]=FitnessSol;
      }
      else trial[i]++;
  }
}

/* Tính xác suất chọn nguồn thức ăn dựa vào fitness */
void CalculateProbabilities()
{
     int i;
     double maxfit;
     maxfit=fitness[0];

     for (i=1;i<FoodNumber;i++)
         if (fitness[i]>maxfit) maxfit=fitness[i];

     for (i=0;i<FoodNumber;i++)
         prob[i]=(0.9*(fitness[i]/maxfit))+0.1;
}

/* Pha ong quan sát: chọn nguồn theo xác suất và cải thiện nó */
void SendOnlookerBees()
{
  int i,j,t;
  i=0;
  t=0;

  while(t<FoodNumber)
  {
      r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
      if(r<prob[i])
      {
          t++;

          /* Chọn tham số để thay đổi */
          r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
          param2change=(int)(r*D);

          /* Chọn nghiệm k khác i */
          r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
          neighbour=(int)(r*FoodNumber);
          while(neighbour==i)
          {
              r=((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
              neighbour=(int)(r*FoodNumber);
          }

          for(j=0;j<D;j++)
              solution[j]=Foods[i][j];

          /* Tạo nghiệm mới */
          r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
          solution[param2change]=Foods[i][param2change]+
                                 (Foods[i][param2change]-Foods[neighbour][param2change])*(r-0.5)*2;

          if (solution[param2change]<lb) solution[param2change]=lb;
          if (solution[param2change]>ub) solution[param2change]=ub;

          ObjValSol=function(solution);
          FitnessSol=CalculateFitness(ObjValSol);

          if (FitnessSol>fitness[i])
          {
              trial[i]=0;
              for(j=0;j<D;j++)
                  Foods[i][j]=solution[j];
              f[i]=ObjValSol;
              fitness[i]=FitnessSol;
          }
          else trial[i]++;
      }

      i++;
      if (i==FoodNumber) i=0;
  }
}

/* Pha ong trinh sát: tìm nghiệm mới nếu trial vượt quá limit */
void SendScoutBees()
{
    int maxtrialindex,i;
    maxtrialindex=0;

    for (i=1;i<FoodNumber;i++)
        if (trial[i]>trial[maxtrialindex])
            maxtrialindex=i;

    if(trial[maxtrialindex]>=limit)
    {
        init(maxtrialindex);
    }
}

/* Chương trình chính của thuật toán ABC */
int main()
{
    int iter,run,j;
    double mean;
    mean=0;
    srand(time(NULL));

    for(run=0;run<runtime;run++)
    {
        initial();
        MemorizeBestSource();

        for (iter=0;iter<maxCycle;iter++)
        {
            SendEmployedBees();
            CalculateProbabilities();
            SendOnlookerBees();
            MemorizeBestSource();
            SendScoutBees();
        }

        for(j=0;j<D;j++)
        {
            printf("GlobalParam[%d]: %f\n",j+1,GlobalParams[j]);
        }

        printf("%d. run: %e \n",run+1,GlobalMin);
        GlobalMins[run]=GlobalMin;
        mean=mean+GlobalMin;
    }

    mean=mean/runtime;
    printf("Means of %d runs: %e\n",runtime,mean);
    getch();
}

/* Hàm Sphere */
double sphere(double sol[D])
{
    int j;
    double top=0;
    for(j=0;j<D;j++)
        top += sol[j]*sol[j];
    return top;
}

/* Hàm Rosenbrock */
double Rosenbrock(double sol[D])
{
    int j;
    double top=0;
    for(j=0;j<D-1;j++)
    {
        top += 100*pow((sol[j+1]-sol[j]*sol[j]),2) + pow((sol[j]-1),2);
    }
    return top;
}

/* Hàm Griewank */
double Griewank(double sol[D])
{
    int j;
    double top1=0, top2=1;

    for(j=0;j<D;j++)
    {
        top1 += sol[j]*sol[j];
        top2 *= cos((sol[j]/sqrt(j+1))*M_PI/180);
    }
    return top1/4000 - top2 + 1;
}

/* Hàm Rastrigin */
double Rastrigin(double sol[D])
{
    int j;
    double top=0;
    for(j=0;j<D;j++)
    {
        top += sol[j]*sol[j] - 10*cos(2*M_PI*sol[j]) + 10;
    }
    return top;
}
