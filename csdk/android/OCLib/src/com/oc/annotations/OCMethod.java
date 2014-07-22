//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc.annotations;

/**
 * 
 * Base OCMethod Interface. Integral piece used to describe a resource's functionality OTA.
 * 
 */
public @interface OCMethod {
  String name() default "";
}
