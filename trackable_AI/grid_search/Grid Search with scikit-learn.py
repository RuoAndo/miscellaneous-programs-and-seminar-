

from sklearn import datasets

iris = datasets.load_iris()
X = iris.data[:,2:]
y = iris.target

from sklearn.model_selection import train_test_split
X_train, X_test, y_train, y_test = train_test_split(X, y, stratify = y,random_state = 7)


from sklearn.neighbors import KNeighborsClassifier

knn_clf = KNeighborsClassifier()


param_grid = {'n_neighbors': list(range(3,9,1))}


from sklearn.model_selection import GridSearchCV, cross_val_score
gs = GridSearchCV(knn_clf,param_grid,cv=10)


gs.fit(X_train, y_train)


gs.best_params_


gs.cv_results_['mean_test_score']
zip(gs.cv_results_['params'],gs.cv_results_['mean_test_score'])




all_scores = []
for n_neighbors in range(3,9,1):
    knn_clf = KNeighborsClassifier(n_neighbors = n_neighbors)
    all_scores.append((n_neighbors, cross_val_score(knn_clf, X_train, y_train, cv=10).mean()))
sorted(all_scores, key = lambda x:x[1], reverse = True)

print(all_scores)
