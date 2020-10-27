#pragma once
#include <iostream>
#include <algorithm>
using namespace std;

struct point { double x, y; };
static bool mult(point sp, point ep, point op) {
	return (sp.x - op.x) * (ep.y - op.y)
		>= (ep.x - op.x) * (sp.y - op.y);
}
static bool operator < (const point& l, const point& r) {
	return l.y < r.y || (l.y == r.y && l.x < r.x);
}
static int graham(point pnt[], int n, point res[]) {
	int i, len, top = 1;
	sort(pnt, pnt + n);
	if (n == 0) return 0; res[0] = pnt[0];
	if (n == 1) return 1; res[1] = pnt[1];
	if (n == 2) return 2; res[2] = pnt[2];
	for (i = 2; i < n; i++) {
		while (top && mult(pnt[i], res[top], res[top - 1]))
			top--;
		res[++top] = pnt[i];
	}
	len = top; res[++top] = pnt[n - 2];
	for (i = n - 3; i >= 0; i--) {
		while (top != len && mult(pnt[i], res[top],
			res[top - 1])) top--;
		res[++top] = pnt[i];
	}
	return top; // 返回凸包中点的个数
}