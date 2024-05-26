/*
 * optimizer_adam.h
 *
 */

#ifndef OPTIMIZER_ADAM_H_
#define OPTIMIZER_ADAM_H_

#include <random>
#include "model.h"
#include "optimizer.h"

int64_t get_rand_range( uint64_t min_val, uint64_t max_val ) {
    // 乱数生成器
    static std::mt19937_64 mt64(0);

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    std::uniform_int_distribution<uint64_t> get_rand_uni_int( min_val, max_val );

    // 乱数を生成
    return get_rand_uni_int(mt64);
}

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

        int tmp; 
        OptimizerAdamParams &op = (OptimizerAdamParams &)opp;

        tmp = get_rand_range(9980,10000);
        // cout << "rand:" << tmp << endl;
        beta2 = (float)tmp/10000; 
        // cout << "beta2:" << beta2 << endl;
        tmp = get_rand_range(88,92); 
        beta1 = (float)tmp/100; 
        // cout << "beta1:" << beta1 << endl; 
        op.adam_w_m.adam2(op.adam_w_v, w->grad, op.ndw, beta1, beta2, lr_f(-lr, epoch), 1e-8);
        w->data.plus(op.ndw, w->data);
    }

};

#endif /* OPTIMIZER_ADAM_H_ */
