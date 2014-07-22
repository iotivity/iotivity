//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc.annotations;

/**
 * 
 * Base OCInterface Interface. Integral piece used to describe a resource's functionality OTA.
 * 
 */
public @interface OCInterface {
  String name() default "";
}
