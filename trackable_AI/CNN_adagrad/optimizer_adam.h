/*
 * optimizer_adam.h
 *
 */

#ifndef OPTIMIZER_ADAM_H_
#define OPTIMIZER_ADAM_H_

#include "model.h"
#include "optimizer.h"

static int counter = 0;

class OptimizerAdamParams : public OptimizerParams {
public:
    cuMat adam_w_m;
    cuMat adam_w_v;

    cuMat dw_tmp;

    cuMat m_h_t;
    cuMat v_h_t;

    cuMat ndw;


    OptimizerAdamParams(int output_units, int input_units) {
        adam_w_m = cuMat(output_units, input_units);
        adam_w_v = cuMat(output_units, input_units);
        m_h_t = cuMat(output_units, input_units);
        v_h_t = cuMat(output_units, input_units);

        dw_tmp = cuMat(output_units, input_units);

        ndw = cuMat(output_units, input_units);
    }

};

class OptimizerAdam: public Optimizer {
public:

    float beta1 = 0.9;
    float beta2 = 0.999;

    OptimizerAdam(Model *model, float lr) : Optimizer(model, lr) {
    }
    OptimizerAdam(Model *model, float lr, float clip_grad_threshold) : Optimizer(model, lr, clip_grad_threshold) {
    }


    OptimizerParams *createOptimizerParams(Variable *v){
        return new OptimizerAdamParams(v->data.rows, v->data.cols);
    }


    float lr_f(float alpha, int epoch){
            float fix1 = 1.0 - std::pow(beta1, epoch);
            float fix2 = 1.0 - std::pow(beta2, epoch);
            return alpha * std::sqrt(fix2) / fix1;
    }

    void update_param(Variable *w, OptimizerParams &opp) {

        OptimizerAdamParams &op = (OptimizerAdamParams &)opp;

        op.adam_w_m.adam2(op.adam_w_v, w->grad, op.ndw, beta1, beta2, lr_f(-lr, epoch), 1e-8);
        w->data.plus(op.ndw, w->data);

	if(counter % 10 == 0)
	  {	   
	    float *tmp;
	    tmp = (float*)calloc(w->data.cols * w->data.rows, sizeof(float));
	    cublasGetVector(w->data.rows * w->data.cols,
			    sizeof(float), w->data.mDevice, 1, tmp, 1 );
	    //cout << "Weight_Data:" << *tmp << endl;


	    float *tmp2;
	    tmp2 = (float*)calloc(w->grad.cols * w->grad.rows, sizeof(float));
	    cublasGetVector(w->grad.rows * w->grad.cols,
			    sizeof(float), w->grad.mDevice, 1, tmp2, 1 );
	    cout << "Weight_Grad:" << *tmp2 << ":Weight_Data:" << *tmp << endl;

	    free(tmp);
	    free(tmp2);
	  }

	counter++;
	    
	/*
	float *tmp3;
	tmp3 = (float*)calloc(op.adam_w_v.cols * op.adam_w_v.rows, sizeof(float));
	cublasGetVector(op.adam_w_v.rows * op.adam_w_v.cols,
			sizeof(float), op->grad.mDevice, 1, tmp2, 1 );
	cout << "[" << counter << "] " << "Weight_Grad:" << *tmp2 << endl;
	free(tmp2);
	*/
	
    }

};

#endif /* OPTIMIZER_ADAM_H_ */
