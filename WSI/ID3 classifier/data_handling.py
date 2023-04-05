import numpy as np
import pandas as pd
from functools import lru_cache

BREAST_CANCER_DATA_FILE = "./data set/breast-cancer.data"
ALL_ATTRIBUTES = ["recurrence", "age", "menopause", "tumor-size", "inv-nodes", "node-caps", "deg-malig", "breast",
              "breast-quad", "irradiat"]
ATTRIBUTES = ["recurrence", "age", "menopause", "tumor-size", "inv-nodes", "node-caps", "deg-malig", "breast",
              "breast-quad"]
CLASSIFIER = "irradiat"


@lru_cache(1)
def get_raw_data():
    return pd.read_csv(BREAST_CANCER_DATA_FILE, names=ALL_ATTRIBUTES)


def split_to_three(data: pd.DataFrame, frac_1, frac_2, frac_3):
    if abs(1 - (frac_1 + frac_2 + frac_3)) > 0.001:
        raise Exception("Fractions dont add up to 1")

    sample_1 = data.sample(frac=frac_1)
    data = data.drop(index=sample_1.index)
    sample_2 = data.sample(frac=(frac_2 / (frac_3 + frac_2)))
    sample_3 = data.drop(index=sample_2.index)

    return sample_1, sample_2, sample_3


def get_breast_cancer_samples(train_fraction, validation_fraction, test_fraction):
    data = get_raw_data()

    irrad_yes = data.groupby(CLASSIFIER).get_group("yes")
    irrad_no = data.groupby(CLASSIFIER).get_group("no")

    yes_train, yes_valid, yes_test = split_to_three(irrad_yes, train_fraction, validation_fraction, test_fraction)
    no_train, no_valid, no_test = split_to_three(irrad_no, train_fraction, validation_fraction, test_fraction)

    train_sample = pd.concat([yes_train, no_train], ignore_index=True)
    valid_sample = pd.concat([yes_valid, no_valid], ignore_index=True)
    test_sample = pd.concat([yes_test, no_test], ignore_index=True)

    return train_sample, valid_sample, test_sample
