#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

//definition of the total number of credit offers
#define offers 71

//definition of the structures used for data processing
//struct for commission
typedef struct credit_commission{
    int commission;
    int low;
    int high;
    int threshhold;
    int annual;
    float effective_commission;
} comm;

//struct used to create output file
typedef struct output_values{
    char credit_name_full[100];
    char bank_name[100];
    char credit_type[100];
    float monthly_pay;
    float monthly_pay_fico;
    int fico;
    char conditions[9000];
    float total_pay;
    float total_pay_fico;
} output_value;

typedef struct special{
    float months[12];
    float months_fico[12];
    float total;
    float total_fico;
    int fico;
} special;

//main struct used for processing info from "data.txt"
typedef struct bank_credit{
    float rate;
    float rate_fico;
    float dae;
    float dae_fico;
    int min_sum;
    int max_sum;
    int min_time_period;
    int max_time_period;
    output_value output;
    comm commission;
} credit;

//function for calculating monthly pay
float pay_per_month(float r, int s, int time, char type[], int max_t, int min_t, int max_s, int min_s){
    //if conditions are not met, return the full sum
    if ((s>max_s) || (s<min_s) || (time>max_t) || (time<min_t)){
        return s;
    }

    if (strcmp(type, "Fixed")==0){
        float p=r/100/12;
        return s*(p+p/(pow(1+p, time)-1));
    }
    else{
        float total_pay=0;
        float b=s/time;
        float P=r/100/12;
        float p=0;
        for (int i=0; i<time; i++){
            p=s*P;
            total_pay=total_pay+p+b;
            s=s-b;
        }
        return total_pay/time;
    }
}

//function to find commission
float dae_difference(float r, float dae, int s, int time, char type[], int max_t, int min_t, int max_s, int min_s){
    return pay_per_month(dae, s, time, type, max_t, min_t, max_s, min_s)-pay_per_month(r, s, time, type, max_t, min_t, max_s, min_s);
}

//function for processing the array of credit offers
void compute_monthly_pay(credit* arr, int num_of_offers, int sum, int time_period, int fico){
    int credits_to_show=num_of_offers;

    for (int i=0; i<num_of_offers; i++){
        //compute monthly pay
        arr[i].output.monthly_pay=pay_per_month(arr[i].rate, sum, time_period, arr[i].output.credit_type, arr[i].max_time_period, arr[i].min_time_period, arr[i].max_sum, arr[i].min_sum);

        //compute fico, rate and dae with respect to fico
        if (fico>680){
            arr[i].rate_fico=arr[i].rate-fico*0.001;
            arr[i].dae_fico=arr[i].dae-fico*0.001;
        }
        else{
            arr[i].rate_fico=arr[i].rate+fico*0.001;
            arr[i].dae_fico=arr[i].dae+fico*0.001;
        }
        arr[i].output.monthly_pay_fico=pay_per_month(arr[i].rate_fico, sum, time_period, arr[i].output.credit_type, arr[i].max_time_period, arr[i].min_time_period, arr[i].max_sum, arr[i].min_sum);
        
        //compute total pay and total pay with respect to fico
        arr[i].output.total_pay=time_period*arr[i].output.monthly_pay;
        arr[i].output.total_pay_fico=time_period*arr[i].output.monthly_pay_fico;

        //compute commission
        arr[i].commission.effective_commission=0;
        if (arr[i].commission.commission==1){
            float computed_commission=0;
            if (sum>arr[i].commission.threshhold){
                arr[i].output.monthly_pay+=arr[i].commission.high/time_period;
                computed_commission+=arr[i].commission.high;
            }
            else{
                arr[i].output.monthly_pay+=arr[i].commission.low/time_period;
                computed_commission+=arr[i].commission.low;
            }
            arr[i].output.monthly_pay+=arr[i].commission.annual/12;
            computed_commission+=arr[i].commission.annual/12*time_period;
            arr[i].commission.effective_commission=computed_commission;
        }
        //compute commission by dae
        arr[i].commission.effective_commission+=dae_difference(arr[i].rate, arr[i].dae, sum, time_period, arr[i].output.credit_type, arr[i].max_time_period, arr[i].min_time_period, arr[i].max_sum, arr[i].min_sum)*time_period;
    }
}

//helper function for sorting the credit offers
int compare_credit(const void *cr1, const void *cr2){
    const credit *credit1=(const credit*)cr1;
    const credit *credit2=(const credit*)cr2;
    return (credit1->output.monthly_pay-credit2->output.monthly_pay);
}

//function to save the results in "output.json" file
void show_best_offer(credit *arr, char *type, int num, int correct, FILE *f){
    int correct_type_printed=0;

    fprintf(f, "[\n");
    for (int i=0; i<num; i++){
        if (strcmp(arr[i].output.credit_type, type)==0){
            fprintf(f, "{\n");
            fprintf(f, "    \"credit_name_full\": \"%s\",\n", arr[i].output.credit_name_full);
            fprintf(f, "    \"bank_name\": \"%s\",\n", arr[i].output.bank_name);
            fprintf(f, "    \"credit_type\": \"%s\",\n", arr[i].output.credit_type);
            fprintf(f, "    \"fico\": %d,\n", arr[i].output.fico);
            fprintf(f, "    \"rate\": %.2f,\n", arr[i].rate);
            fprintf(f, "    \"rate_fico\": %.2f,\n", arr[i].rate_fico);
            fprintf(f, "    \"dae\": %.2f,\n", arr[i].dae);
            fprintf(f, "    \"dae_fico\": %.2f,\n", arr[i].dae_fico);
            fprintf(f, "    \"conditions\": \"%s\",\n", arr[i].output.conditions);
            fprintf(f, "    \"commission\": %.2f,\n", arr[i].commission.effective_commission);
            fprintf(f, "    \"monthly_pay\": %.2f,\n", arr[i].output.monthly_pay);
            fprintf(f, "    \"monthly_pay_fico\": %.2f,\n", arr[i].output.monthly_pay_fico);
            fprintf(f, "    \"total_pay\": %.2f,\n", arr[i].output.total_pay);
            fprintf(f, "    \"total_pay_fico\": %.2f\n", arr[i].output.total_pay_fico);
            correct_type_printed++;
            fprintf(f, "}%s\n", ((i<num-1)&&(correct_type_printed!=correct)) ? "," : "");
        }
    }
    fprintf(f, "]\n");

    //confirmation of success in terminal
    printf("Finished successfully.\n");
}

//function to compute fico based on the survey data
int compute_fico(int* arr){
    int fico=650;
    switch (arr[0]){
        case 1: fico+=10.5;
        break;
        case 2: fico+=35;
        break;
        case 3: fico+=15.5;
        break;
        case 4: fico+=40.5;
        break;
    }
    switch (arr[1]){
        case 1: fico+=40.5;
        break;
        case 2: fico+=25.5;
        break;
        case 3: fico+=15.5;
        break;
        case 4: fico+=10.5;
        break;
    }
    switch (arr[2]){
        case 1: fico+=69;
        break;
        case 2: fico+=50;
        break;
        case 3: fico+=30;
        break;
        case 4: fico+=10;
        break;
    }
    switch (arr[3]){
        case 1: fico+=34;
        break;
        case 2: fico+=26;
        break;
        case 3: fico+=15;
        break;
        case 4: fico+=9;
        break;
    }
    switch (arr[4]){
        case 1: fico+=23;
        break;
        case 2: fico+=18;
        break;
        case 3: fico+=12;
        break;
        case 4: fico+=5;
        break;
    }
    switch (arr[5]){
        case 1: fico+=3;
        break;
        case 2: fico+=15;
        break;
        case 3: fico+=23;
        break;
    }
    return fico;
}

//function to include the fico score in the output file
void include_fico(credit *arr, int *surv, int num_of_offers){
    int fico=compute_fico(surv);
    for (int i=0; i<num_of_offers; i++){
        arr[i].output.fico=fico;
    }
}

//function to check if credit conditions are satisfied
int find_acceptable_offers(credit* arr, int sum, int num_of_offers){
    int acceptable=num_of_offers;
    for (int i=0; i<num_of_offers; i++){
        if (arr[i].output.monthly_pay>=sum){
            acceptable--;
        }
    }
    return acceptable;
}

//function to find how many offers match the desired type
int correct_type(credit* arr, char *type, int num_of_offers){
    int correct=0;
    for (int i=0; i<num_of_offers; i++){
        if (strcmp(arr[i].output.credit_type, type)==0){
            correct++;
        }
    }
    return correct;
}

//function to get credit info from the database
void get_data(credit* arr, int num_of_offers, FILE *f){
    for (int i=0; i<num_of_offers; i++){

        int items_read=fscanf(f, "%99[^,],%99[^,],%99[^,],%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d,%9999[^\n]%*c",
            arr[i].output.credit_name_full,
            arr[i].output.bank_name,
            arr[i].output.credit_type,
            &arr[i].min_time_period,
            &arr[i].max_time_period,
            &arr[i].min_sum,
            &arr[i].max_sum,
            &arr[i].rate,
            &arr[i].dae,
            &arr[i].commission.commission,
            &arr[i].commission.low,
            &arr[i].commission.high,
            &arr[i].commission.threshhold,
            &arr[i].commission.annual,
            arr[i].output.conditions);

        if (items_read!=15){
            printf("file items do not match\n");
        }
    }
}

void get_special(FILE *f, float *arr){
    int i=0;
    while (fscanf(f, "%f,", &arr[i])==1 && i<12){
        i++;
    }
}

float special_pay(float r, int s, int t){
    float p=r/100/12;
    return s*(p+p/(pow(1+p, t)-1));
}

float special_fico(int fico, float rate){
    if (fico>680){
        return rate-fico*0.001;
    }
    else{
        return rate+fico*0.001;
    }
}

void show_special(FILE *f, credit item, float* rates, int sum, int time){
    int correct_type_printed=0;
    fprintf(f, "[\n");
    for (int i=0; i<12; i++){
        float total=special_pay(rates[i], sum, time)*time;
        float total_dae=special_pay(special_fico(item.output.fico, rates[i]), sum, time)*time;

        fprintf(f, "{\n");
        fprintf(f, "    \"credit_name_full\": \"%s\",\n", item.output.credit_name_full);
        fprintf(f, "    \"bank_name\": \"%s\",\n", item.output.bank_name);
        fprintf(f, "    \"credit_type\": \"%s\",\n", item.output.credit_type);
        fprintf(f, "    \"fico\": %d,\n", item.output.fico);

        fprintf(f, "    \"rate\": %.2f,\n", rates[i]);
        fprintf(f, "    \"rate_fico\": %.2f,\n", special_fico(item.output.fico, rates[i]));
        fprintf(f, "    \"commission\": %.2f,\n", fabs(total_dae-total));

        fprintf(f, "    \"dae\": %.2f,\n", item.dae);
        fprintf(f, "    \"dae_fico\": %.2f,\n", item.dae_fico);
        fprintf(f, "    \"conditions\": \"%s\",\n", item.output.conditions);
        
        fprintf(f, "    \"monthly_pay\": %.2f,\n", special_pay(rates[i], sum, time));
        fprintf(f, "    \"monthly_pay_fico\": %.2f,\n", special_pay(special_fico(item.output.fico, rates[i]), sum, time));
        fprintf(f, "    \"total_pay\": %.2f,\n", total);
        fprintf(f, "    \"total_pay_fico\": %.2f\n", total_dae);
        correct_type_printed++;
        fprintf(f, "}%s\n", ((i<12-1)&&(correct_type_printed!=12)) ? "," : "");
    }
    fprintf(f, "]\n");
}

int main(){

    int sum=0;
    int time_period=0;
    
    credit *list_of_credits=(credit*)malloc(offers*sizeof(credit));
    int *survey=(int*)malloc(6*sizeof(int));
    float *special_rates=(float*)malloc(12*sizeof(float));
    char *credit_type=(char*)malloc(50*sizeof(char));
    
    FILE *fdata=fopen("data.txt", "r");
    if (fdata==NULL){
        perror("Error opening file data.txt");
        return 1;
    }

    FILE *finput=fopen("input.txt", "r");
    if (finput==NULL){
        perror("Error opening file input.txt");
        return 1;
    }

    FILE *finput2=fopen("input2.txt", "r");
    if (finput2==NULL){
        perror("Error opening file input2.txt");
        return 1;
    }

    FILE *foutput=fopen("output.json", "w");
    if (foutput==NULL){
        perror("Error opening file output.json");
        return 1;
    }

    FILE *fspecial=fopen("special.json", "w");
    if (fspecial==NULL){
        perror("Error opening file special.json");
        return 1;
    }
    
    get_data(list_of_credits, offers, fdata);
    fscanf(finput, "%d,%d,%d,%d,%d,%d,%d,%d,%49[^\n]%*c", &sum, &time_period, &survey[0], &survey[1], &survey[2], &survey[3], &survey[4], &survey[5], credit_type);
    get_special(finput2, special_rates);

    compute_monthly_pay(list_of_credits, offers, sum, time_period, compute_fico(survey));
    include_fico(list_of_credits, survey, offers);
    qsort(list_of_credits, offers, sizeof(credit), compare_credit);

    int acceptable=find_acceptable_offers(list_of_credits, sum, offers);
    int correct=correct_type(list_of_credits, credit_type, acceptable);
    show_best_offer(list_of_credits, credit_type, acceptable, correct, foutput);
    show_special(fspecial, list_of_credits[0], special_rates, sum, time_period);

    fclose(fdata);
    fclose(finput);
    fclose(foutput);
    fclose(finput2);
    fclose(fspecial);
    free(special_rates);
    free(list_of_credits);
    free(survey);
    free(credit_type);
}