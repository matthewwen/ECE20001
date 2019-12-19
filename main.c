#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "json.h"

typedef struct{int count; jobject * head;}class;
class scan_data();

typedef struct{int * index; float * score;}grade;
grade get_grade(class c);

// ************** EDIT THIS METHOD ************* //
void the_curve(class c, grade g) {
	int class_size = c.count;

	int brank = class_size * 0.2;
	int crank = class_size * 0.4;
	int drank = class_size * 0.7;
	int frank = class_size * 0.9;

	// display rank
	int i, j;
	for (i = 0; i < class_size; i++) {
		if (i == 0) {
			printf("A Rank\n");
		}
		else if (i == brank) {
			printf("B Rank\n");
		}
		else if (i == crank) {
			printf("C Rank\n");
		}
		else if (i == drank) {
			printf("D Rank\n");
		}
		else if (i == frank) {
			printf("F Rank\n");
		}

		jobject * curr = c.head;

		// go to student
		int studentidx = g.index[i];
		for (j = 0; j < studentidx; j++) {
			curr = curr->next;
		}

		// display name
		jlist * studentdata = curr->data.list;
		printf("name: %s %s, e1: %.2lf, e2: %.2lf, e3: %.2lf, f: %.2lf, hw: %.2lf, score: %.2f\n", studentdata->next->data.txt, 
				                                                    studentdata->data.txt, 
										    studentdata->next->next->data.flt, 
										    studentdata->next->next->next->data.flt, 
										    studentdata->next->next->next->next->data.flt, 
										    studentdata->next->next->next->next->next->data.flt, 
										    studentdata->next->next->next->next->next->next->data.flt, 
										    g.score[i]);
	}

	// average and median
	float average = 0;
	for (i = 0; i < class_size; i++) {
		average = average + g.score[i];
	}
	average = average / class_size; 
	printf("Number of Students: %d, Average Score: %f, Median Score: %f\n", class_size, average, g.score[class_size / 2]);
}

int main(int argc, char* argv[]) {
	class c2 = scan_data();
	grade g2 = get_grade(c2);
	the_curve(c2, g2);
	free_json(&c2.head);
	free(g2.index);
	free(g2.score);
	return EXIT_SUCCESS;
}

grade get_grade(class c) {
	int class_size = c.count;
	int * index;
	float * score;
	size_t alloc_size = class_size * sizeof(*index);

	index = malloc(alloc_size);
	memset(index, 0, alloc_size);
	alloc_size = class_size * sizeof(*score);
	score = malloc(alloc_size);
	memset(score, 0, alloc_size);

	// get scores
	int i = 0;
	jobject * curr;
	for (curr = c.head; curr != NULL; curr = curr->next) {
		jlist * l = curr->data.list;
		l = l->next->next; // skip the names
		float e1 = l->data.flt;
		l = l->next;
		float e2 = l->data.flt; 
		l = l->next;
		float e3 = l->data.flt;
		l = l->next;
		float f = l->data.flt;
		l = l->next;
		float hw = l->data.flt;
		assert(l->next == NULL);

		// calculate score
		float m1 = (0.15 * hw) + (0.2 * e1) + (0.2 * e2) + (0.2 * e3) + (0.25 * f);
		float m2 = (0.1 * hw) + (0.35 * f);
		do {
			float min, summax;
			min    = e3;
			summax = e1 + e2;
			if (e1 < e2 && e1 < e3) {
				min    = e1;
				summax = e2 + e3;
			}
			else if (e2 < e1 && e2 < e3) {
				min    = e2;
				summax = e1 + e3;
			}
			m2 = m2 + (min * 0.05) + (0.25 * summax);
		}while(false);

		//printf("name: %s, e1: %f, e2: %f, e3: %f, f: %f, hw: %f, m1: %f, m2: %f, min: %f, summax: %f\n", curr->data.list->data.txt, 
				//e1, e2, e3, f, hw, m1, m2, min, summax);

		score[i] = m1 > m2 ? m1: m2;
		// extra credit for all
		score[i] += 0.02;
		index[i] = i;
		i++;
	}


	// sort method -> heap in decending order
	// append elements to heap
	for (i = 0; i < class_size; i++) {
		int cpyi = i;
		int nexti;
		assert(cpyi != nexti);
		bool is_found = false;
		for (; (nexti = (cpyi - 1)) >= 0; cpyi = nexti) {
			nexti = nexti / 2;
			assert(cpyi != 0);
			assert(cpyi != nexti);
			if ((is_found = !(score[nexti] < score[cpyi]))) {
				float tempscore = score[nexti];
				score[nexti]    = score[cpyi];
				score[cpyi]     = tempscore;

				int tempidx  = index[nexti];
				index[nexti] = index[cpyi];
				index[cpyi]  = tempidx;
			}
		}
	}


	// remove elements from heap
	for (i = i - 1; i > 0; i--) {
		float tempscore = score[0];
		score[0]        = score[i];
		score[i]        = tempscore;

		int tempidx = index[0];
		index[0] = index[i];
		index[i] = tempidx;

		int nextli, nextri;
		bool is_done = false;
		int cpyi, swapidx = 0;
		for (cpyi = 0; !is_done && (swapidx != -1); cpyi = swapidx) {
			nextli = 2 * cpyi + 1;
			nextri = (is_done = (nextli == (i - 1))) ? nextli: nextli + 1;
			if (!(is_done = (nextli >= i))) {
				assert(nextli < i);
				assert(nextri < i);
				swapidx = -1;
				if (score[cpyi] > score[nextli] && score[nextli] < score[nextri]) {
					swapidx = nextli;
				}
				else if (score[cpyi] > score[nextri]) {
					swapidx = nextri;
				}

				if (swapidx != -1) {
					float tempscore = score[swapidx];
					score[swapidx]  = score[cpyi];
					score[cpyi]     = tempscore;

					int tempidx    = index[swapidx];
					index[swapidx] = index[cpyi];
					index[cpyi]    = tempidx;
				}
			}
		}
	}

	return (grade) {.index = index, .score = score};
}

class scan_data() {
	int count = 0;
	char countstr[12];
	memset(countstr, 0, sizeof(countstr));

	FILE * fp = fopen("data.txt", "r");
	char first_name[100];
	char last_name[100];

	bool error = false;
	jobject * root = NULL;

	while (!feof(fp) && !error) {
		error = fscanf(fp, "%s", first_name) == 1;
		error = fscanf(fp, "%s", last_name)  == 1;
		int i;
		double score;

		data_t userdata;
		jlist * userhead = NULL;

		// add first name
		char * allocstr = malloc((strlen(first_name) + 1) * sizeof(*allocstr));
		userdata.txt = allocstr;
		strcpy(allocstr, first_name);
		append_jlist(&userhead, TEXT, userdata);

		// add last name
		allocstr = malloc((strlen(last_name) + 1) * sizeof(*allocstr));
		userdata.txt = allocstr;
		strcpy(allocstr, last_name);
		append_jlist(&userhead, TEXT, userdata);

		// add blackboard scores
		float hw = 0;
		float hwtotal = 32 * 20;
		for (i = 0; i < 39; i++) {
			error = fscanf(fp, "%lf", &score) == 1;
			if (i == 15) {
				userdata.flt = score / 100;
				append_jlist(&userhead, FLOAT, userdata);
			}
			else if (i == 22) {
				userdata.flt = score / 100;
				append_jlist(&userhead, FLOAT, userdata);
			}
			else if (i == 31) {
				userdata.flt = score / 100;
				append_jlist(&userhead, FLOAT, userdata);
			}
			else if (i == 38) {
				userdata.flt = score / 100;
				append_jlist(&userhead, FLOAT, userdata);
			}
			else {
				hw = hw + score;
			}
		}
		error = fscanf(fp, "%lf", &score) == 1; // get rid of space
		userdata.flt = hw / hwtotal;
		append_jlist(&userhead, FLOAT, userdata);

		// creating object for user
		userdata.list = userhead;
		sprintf(countstr, "%d", count++);
		append_jobject(&root, countstr, LIST, userdata);
	}
	fclose(fp);
	return (class) {.count = count, .head = root};
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
