/*
 * optimizer_adagrad.h
 *
 */

#ifndef OPTIMIZER_ADAGRAD_H_
#define OPTIMIZER_ADAGRAD_H_

#include "model.h"
#include "optimizer.h"

static int counter_adagrad = 0;

class OptimizerAdagradParams : public OptimizerParams {
public:
    cuMat ndw;
    cuMat g2;


    OptimizerAdagradParams(int output_units, int input_units) {


        ndw = cuMat(output_units, input_units);
        g2 = cuMat(output_units, input_units);
    }
};

class OptimizerAdagrad : public Optimizer {
public:


    OptimizerAdagrad(Model *model, float lr) : Optimizer(model, lr) {
    }
    OptimizerAdagrad(Model *model, float lr, float clip_grad_threshold) : Optimizer(model, lr, clip_grad_threshold) {
    }

    OptimizerParams *createOptimizerParams(Variable *v){
        return new OptimizerAdagradParams(v->data.rows, v->data.cols);
    }



    void update_param(Variable *w, OptimizerParams &opp) {

        OptimizerAdagradParams &op = (OptimizerAdagradParams &)opp;

        op.g2 += w->grad * w->grad;

        cuMat tmp = op.g2.sqrt();
        tmp = w->grad / tmp;

        tmp.mul(-lr, op.ndw);

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
	
    }

};

#endif /* OPTIMIZER_ADAGRAD_H_ */
