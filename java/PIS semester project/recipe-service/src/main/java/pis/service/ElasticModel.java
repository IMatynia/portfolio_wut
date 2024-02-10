package pis.service;

import org.springframework.beans.factory.annotation.Value;
import pis.entity.Recipe;

public interface ElasticModel {


    public void insert(Recipe recipe) throws Exception;
    public void delete(Long recipie_id) throws Exception;
}
