#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "json.h"

void scan_datav2();

int main(int argc, char* argv[]) {
	scan_datav2();
	return EXIT_SUCCESS;
}

void scan_datav2() {
	FILE * fp = fopen("section3.txt", "r");
	char first_name[100];
	char last_name[100];

	jlist * actual = NULL;
	bool error = true;

	while (!feof(fp)) {
		error = fscanf(fp, "%s", first_name) == 1;
		error = fscanf(fp, "%s", last_name)  == 1;
		int i;
		float score;

		// add first name
		printf("%s\n%s\n", first_name, last_name);

		// filter out unused
		error = fscanf(fp, "%s", first_name) == 1; // marito
		error = fscanf(fp, "%f", &score) == 1; // id
		error = fscanf(fp, "%s", first_name) == 1; //date month
		error = fscanf(fp, "%s", first_name) == 1; //date day, 
		error = fscanf(fp, "%s", first_name) == 1; //date year
		error = fscanf(fp, "%s", first_name) == 1; // available
		error = fscanf(fp, "%f", &score)     == 1; // score 1
		error = fscanf(fp, "%f", &score)     == 1; // score 2
		error = fscanf(fp, "%f", &score)     == 1; // score 3
		error = fscanf(fp, "%f", &score)     == 1; // score 4

		for (i = 0; i < 57; i++) {
			if (i == 23 || i == 28  || i == 29 || i == 30 || i == 43  || i == 44  || i == 45 || i == 53 || i == 54 || i == 55) {
				error = fscanf(fp, "%s", first_name) == 1;
			}
			else if (i < 8) {

			}
			else if (i < 11) {
				error = fscanf(fp, "%f", &score) == 1;
				printf("%.2f\n", 0.0);
			}
			else {
				error = fscanf(fp, "%f", &score) == 1;
				printf("%.2f\n", score);
			}
		}

		// fiter out more data
		error = fscanf(fp, "%f", &score) == 1; // final score
		data_t data;
		data.flt = score;
		append_jlist(&actual, FLOAT, data);
		printf("\n");
	}
	fclose(fp);

	fp = fopen("actual.txt", "w");
	int i;
	for (i = 0; i < 83; i++) {
		fprintf(fp, "0\n");
	}
	data_t data;
	data.list = actual;
	jobject * obj = create_jobject("Scores", LIST, data);
	jlist * curr = obj->data.list;
	for (;curr != NULL; curr = curr->next) {
		fprintf(fp, "%lf\n", curr->data.flt);
	}
	fclose(fp);
	free_json(&obj);
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */