#include <bits/stdc++.h>
using namespace std;

#define FOOD_NUMBER 20   // S? ngu?n th?c an
#define D 2              // S? chi?u nghi?m (x, y)
#define LOWER -10        // Gi?i h?n du?i
#define UPPER 10         // Gi?i h?n trên  
#define LIMIT 30         // Ngu?ng scout
#define MAXCYCLE 200     // S? vòng l?p

struct FoodSource {
    vector<double> x;   // V? trí nghi?m [x, y]
    double f;           // Giá tr? hàm m?c tiêu
    double fitness;     // Ð? phù h?p
    int trial;          // S? l?n th?t b?i

    FoodSource() {
        x.assign(D, 0.0);
        f = 0;
        fitness = 0;
        trial = 0;
    }
};

FoodSource foods[FOOD_NUMBER];
FoodSource bestSolution;

/// HÀM M?C TIÊU: Gi?i phuong trình x² + y² - 4x - 6y + 13 = 0
/// Nghi?m dúng: x = 2, y = 3
double objectiveFunction(const vector<double>& x) {
    double result = x[0]*x[0] + x[1]*x[1] - 4*x[0] - 6*x[1] + 13;
    return abs(result);  // Tìm nghi?m sao cho ˜ 0
}

// Chuy?n f ? fitness (f càng nh? ? fitness càng l?n)
double calculateFitness(double f) {
    return 1.0 / (1.0 + f);
}

// Kh?i t?o ng?u nhiên
void initialize() {
    for (int i = 0; i < FOOD_NUMBER; i++) {
        for (int j = 0; j < D; j++) {
            foods[i].x[j] = LOWER + (double)rand() / RAND_MAX * (UPPER - LOWER);
        }
        foods[i].f = objectiveFunction(foods[i].x);
        foods[i].fitness = calculateFitness(foods[i].f);
        foods[i].trial = 0;

        if (i == 0 || foods[i].fitness > bestSolution.fitness) {
            bestSolution = foods[i];
        }
    }
}

// Pha ong th?
void employedBeePhase() {
    for (int i = 0; i < FOOD_NUMBER; i++) {
        FoodSource newFood = foods[i];
        
        int j = rand() % D;
        int k;
        do { k = rand() % FOOD_NUMBER; } while (k == i);
        
        double phi = ((double)rand() / RAND_MAX) * 2 - 1;
        newFood.x[j] = foods[i].x[j] + phi * (foods[i].x[j] - foods[k].x[j]);
        newFood.x[j] = max((double)LOWER, min((double)UPPER, newFood.x[j]));
        
        newFood.f = objectiveFunction(newFood.x);
        newFood.fitness = calculateFitness(newFood.f);
        
        if (newFood.fitness > foods[i].fitness) {
            foods[i] = newFood;
            foods[i].trial = 0;
        } else {
            foods[i].trial++;
        }
    }
}

// Pha ong quan sát
void onlookerBeePhase() {
    double sumFit = 0;
    for (int i = 0; i < FOOD_NUMBER; i++) 
        sumFit += foods[i].fitness;

    int count = 0;
    while (count < FOOD_NUMBER) {
        double r = ((double)rand() / RAND_MAX) * sumFit;
        double accum = 0;
        int i = 0;
        
        for (i = 0; i < FOOD_NUMBER; i++) {
            accum += foods[i].fitness;
            if (accum >= r) break;
        }
        
        FoodSource newFood = foods[i];
        int j = rand() % D;
        int k;
        do { k = rand() % FOOD_NUMBER; } while (k == i);
        
        double phi = ((double)rand() / RAND_MAX) * 2 - 1;
        newFood.x[j] = foods[i].x[j] + phi * (foods[i].x[j] - foods[k].x[j]);
        newFood.x[j] = max((double)LOWER, min((double)UPPER, newFood.x[j]));
        
        newFood.f = objectiveFunction(newFood.x);
        newFood.fitness = calculateFitness(newFood.f);
        
        if (newFood.fitness > foods[i].fitness) {
            foods[i] = newFood;
            foods[i].trial = 0;
        } else {
            foods[i].trial++;
        }
        
        count++;
    }
}

// Pha ong trinh sát
void scoutBeePhase() {
    for (int i = 0; i < FOOD_NUMBER; i++) {
        if (foods[i].trial > LIMIT) {
            for (int j = 0; j < D; j++) {
                foods[i].x[j] = LOWER + (double)rand() / RAND_MAX * (UPPER - LOWER);
            }
            foods[i].f = objectiveFunction(foods[i].x);
            foods[i].fitness = calculateFitness(foods[i].f);
            foods[i].trial = 0;
        }
    }
}

// C?p nh?t nghi?m t?t nh?t
void memorizeBest() {
    for (int i = 0; i < FOOD_NUMBER; i++) {
        if (foods[i].fitness > bestSolution.fitness) {
            bestSolution = foods[i];
        }
    }
}

int main() {
    srand(time(NULL));
    
    cout << "=== GIAI PHUONG TRINH 2 AN BANG ABC ===\n";
    cout << "Phuong trinh: x² + y² - 4x - 6y + 13 = 0\n";
    cout << "Nghiem dung: x = 2, y = 3\n\n";
    
    initialize();

    cout << "Bat dau toi uu...\n";
    for (int cycle = 0; cycle < MAXCYCLE; cycle++) {
        employedBeePhase();
        onlookerBeePhase(); 
        scoutBeePhase();
        memorizeBest();
    }
    
    cout << "Ket qua:\n";
    cout << "x = " << bestSolution.x[0] << "\n";
    cout << "y = " << bestSolution.x[1] << "\n";
    cout << "f(min) = " << bestSolution.f << endl;

    // Ki?m tra
    double check = bestSolution.x[0]*bestSolution.x[0] + bestSolution.x[1]*bestSolution.x[1] 
                   - 4*bestSolution.x[0] - 6*bestSolution.x[1] + 13;
    cout << "Kiem tra: " << check << " (˜ 0 la dung)" << endl;

    return 0;
}
