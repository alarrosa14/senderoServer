//
//  LinearColorCalibrator.cpp
//  LEDColorCalibrator
//
//  Created by Tomas Laurenzo on 4/20/13.
//
//

#include "LinearColorCalibrator.h"


LinearColorCalibrator::LinearColorCalibrator(){
    gammaCorrecting = false;
    isCalculated = true;
    
}

void LinearColorCalibrator::useGamma() {
    gammaCorrecting = true;
}

void LinearColorCalibrator::dontUseGamma() {
    gammaCorrecting = false;
}

void LinearColorCalibrator::setGamma (float gamma) {
    this->gamma = gamma;
}

bool LinearColorCalibrator::calculateCalibration() {
    if (pairs.size() < 3) {
        isCalculated = false;
        return false;
    }
    
    ofColor v0 = pairs[pairs.size() - 1].first;
    ofColor v1 = pairs[pairs.size() - 2].first;
    ofColor v2 = pairs[pairs.size() - 3].first;
    
    ofColor w0 = pairs[pairs.size() - 1].second;
    ofColor w1 = pairs[pairs.size() - 2].second;
    ofColor w2 = pairs[pairs.size() - 3].second;
    
    double *A;
    A = new double[81];
    
    // matrix in COLUMN MAJOR ORDER
    
    // columns 1..3
    int i = 0;
    A[i++] = v0.r;
    A[i++] = v1.r;
    A[i++] = v2.r;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    
    A[i++] = v0.g;
    A[i++] = v1.g;
    A[i++] = v2.g;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    
    A[i++] = v0.b;
    A[i++] = v1.b;
    A[i++] = v2.b;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;

    
    //columns 4..6
    
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = v0.r;
    A[i++] = v1.r;
    A[i++] = v2.r;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = v0.g;
    A[i++] = v1.g;
    A[i++] = v2.g;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = v0.b;
    A[i++] = v1.b;
    A[i++] = v2.b;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    
    //columns 7..9
    
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = v0.r;
    A[i++] = v1.r;
    A[i++] = v2.r;
    
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = v0.g;
    A[i++] = v1.g;
    A[i++] = v2.g;
    
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = 0;
    A[i++] = v0.b;
    A[i++] = v1.b;
    A[i++] = v2.b;

    double *b;
    b = new double[9]; // w1, w2, w3
    i = 0;
    b[i++] = w0[0];
    b[i++] = w0[1];
    b[i++] = w0[2];
    b[i++] = w1[0];
    b[i++] = w1[1];
    b[i++] = w1[2];
    b[i++] = w2[0];
    b[i++] = w2[1];
    b[i++] = w2[2];

    
    // solving the linear system AX = b
    
    int N = 9; // number of columns of A
    int NRHS = 1; // number of columns of b
    int LDA = 9; // number of rows of A
    int IPIV[9]; // pivot indices
    int LDB = 9; // number of rows of b
    int INFO = 0;

    //INFO = clapack_dgesv((CBLAS_ORDER) 101, N, NRHS, A, LDA, IPIV, b, LDB);
    dgesv_(&N, &NRHS, A, &LDA, IPIV, b, &LDB, &INFO);

    if (INFO == 0) cout << endl << "dgesv_: OK";
    if (INFO < 0) cout << "dgesv_: the element " << INFO << " is invalid";
    if (INFO > 0) cout << "dgesv_: the LU decomposition returned 0 ";
    cout << endl << endl;
    
    /* 
     
     __CLPK_integer is long *
     __CLPK_doublereal is double *
     

     dgesv_(__CLPK_integer *n, __CLPK_integer *nrhs, __CLPK_doublereal *a, __CLPK_integer *lda, __CLPK_integer *ipiv,
     __CLPK_doublereal *b, __CLPK_integer *ldb, __CLPK_integer *info)
    
    void dgesv_(const int *N, const int *nrhs, double *A, const int *lda, int
    *ipiv, double *b, const int *ldb, int *info);
    
    Notice that all parameters are passed by reference. That's the good old Fortran way. The meanings of the parameters are:
        N       number of columns of A
        nrhs	number of columns of b, usually 1
        lda     number of rows (Leading Dimension of A) of A
        ipiv	pivot indices
        ldb     number of rows of b

    */

    i = 0;
    T[0][0] = b[i++];
    T[1][0] = b[i++];
    T[2][0] = b[i++];
    T[0][1] = b[i++];
    T[1][1] = b[i++];
    T[2][1] = b[i++];
    T[0][2] = b[i++];
    T[1][2] = b[i++];
    T[2][2] = b[i++];
    
    cout << "Transformation matrix: " << endl;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j ++) {
            cout << "\t" << T[i][j];
        }
        cout << endl;
    }
    cout << endl;
    
    isCalculated = true;
    
    delete[]A;
    delete[]b;
    
}

ofColor LinearColorCalibrator::getCalibratedColor(ofColor color) {
    if (!isCalculated) calculateCalibration();
    if (!isCalculated) return color;
    
    // calibrated color = T * color
    ofColor res;
    
    res = res.black;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            res[i] += color[i] * T[j][i];
        }
    }
    
    if (gammaCorrecting) {
        for (int i = 0; i < 3; i++) {
            float f = (int)res[i] / 255.0f;
            float r = pow(f, gamma);
            int g = r * 255.0f;               
            int o = f * 255.0f;
            res[i] = g;            
        }
    }
    
    return res;    
}

float LinearColorCalibrator::getGamma() {
    return gamma;
}

void LinearColorCalibrator::addEquivalentPair(ofColor first, ofColor second) {
    
    // we assume lineal, because we are in a hurry and non linear is complicated
    // todo: change it to non linear transformation using little CMS
    // the idea is, we create a profile for the LEDs and a profile for the monitor and use little CMS to make the
    // transformation between them
    
//    pair <first, second> p;
    
    pairs.push_back(pair<ofColor, ofColor>(first, second));
    
    isCalculated = false;
}

void LinearColorCalibrator::reset() {
    
    pairs.clear();
    
    isCalculated = false;
}

