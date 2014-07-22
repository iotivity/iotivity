//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc.annotations;

/**
 * 
 * Base OCProperty Interface. Integral piece used to describe a resource's functionality OTA.
 * 
 */
public @interface OCProperty {
  String name() default "";

  String type() default "";
}
