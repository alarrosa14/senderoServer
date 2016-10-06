
float additiveBlend (float v1, float v2, float blendFactor){
    float res=(v1 * (1.0f- blendFactor)) + (v2 * blendFactor);
    if (res>255.0f){
        res=255.0f;
    }
    return res;
}

kernel void additive(global float* appFrame, global float* frame1, global float* frame2, global float* frame3,
                     global float* frame4, global float* frame5, global float* frame6, global float* frame7, global float* frame8,
                     float f1Enabled, float f2Enabled, float f3Enabled, float f4Enabled, float f5Enabled,
                     float f6Enabled, float f7Enabled, float f8Enabled, float blend1, float blend2,
                     float blend3, float blend4, float blend5 , float blend6, float blend7, float blend8,
                     float decay,
                     global float* output){
    
    
    size_t i = get_global_id(0);
    //printf ("%d \n", i);
    //decay
    output[i]=additiveBlend(appFrame[i],0.0f,decay);
    // blend frame1
    if (f1Enabled==1){
        output[i]=additiveBlend(output[i],frame1[i],blend1);
    }
    if (f2Enabled==1){
        output[i]=additiveBlend(output[i],frame2[i],blend2);
    }
    if (f3Enabled==1){
        output[i]=additiveBlend(output[i],frame3[i],blend3);
    }
    if (f4Enabled==1){
        output[i]=additiveBlend(output[i],frame4[i],blend4);
    }
    if (f5Enabled==1){
         output[i]=additiveBlend(output[i],frame5[i],blend5);
    }
    if (f6Enabled==1){
         output[i]=additiveBlend(output[i],frame6[i],blend6);
    }
    if (f7Enabled==1){
         output[i]=additiveBlend(output[i],frame7[i],blend7);
    }
    if (f8Enabled==1){
         output[i]=additiveBlend(output[i],frame8[i],blend8);
    }

}


