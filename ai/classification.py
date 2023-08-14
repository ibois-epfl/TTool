"""
Classical approach with a linear stochastic gradient descent classifier.
"""

import pickle

import matplotlib.pyplot as plt
import numpy as np
import sklearn
import sklearn.feature_selection
import sklearn.linear_model
import sklearn.model_selection
import sklearn.preprocessing

with open("../../data/features.pkl", "rb") as f:
    data = pickle.load(f)

classes = data["classes"]
X = data["features"]


label_encoder = sklearn.preprocessing.LabelEncoder()
y = label_encoder.fit_transform(classes)

X_train, X_test, y_train, y_test = sklearn.model_selection.train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y, shuffle=True
)

scaler = sklearn.preprocessing.StandardScaler()
scaler.fit(X_train)
X_train = scaler.transform(X_train)
X_test = scaler.transform(X_test)

# var = np.var(X_train, axis=0)
# plt.hist(var, bins=100)
# plt.show()
# quit()

selector = sklearn.feature_selection.VarianceThreshold(threshold=0.02)
selector.fit(X_train)
X_train = selector.transform(X_train)
X_test = selector.transform(X_test)

sgd_clf = sklearn.linear_model.SGDClassifier(
    random_state=42, max_iter=1000, tol=1e-3, n_jobs=8
)
sgd_clf.fit(X_train, y_train)

with open("../../data/model.pkl", "wb") as f:
    pickle.dump(sgd_clf, f)

with open("../../data/model.pkl", "rb") as f:
    sgd_clf = pickle.load(f)

y_pred = sgd_clf.predict(X_test)
score = sgd_clf.score(X_test, y_test)
print(score)

labels = label_encoder.inverse_transform(sgd_clf.classes_)
confusion_matrix = sklearn.metrics.confusion_matrix(y_test, y_pred)
confusion_matrix = confusion_matrix / np.sum(confusion_matrix, axis=1)[:, None]
print(np.sum(confusion_matrix, axis=1))
disp = sklearn.metrics.ConfusionMatrixDisplay(
    confusion_matrix=confusion_matrix, display_labels=labels
)
disp.plot()
plt.show()
