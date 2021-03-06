/*
 *  CrashReporter.m
 *  Plex
 *
 *  Created by Faustino Osuna on 7/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#import "SmartCrashReportsInstall.h"

int main(int argc, char** argv)
{
  Boolean authenticationWillBeRequired = FALSE;
  if (UnsanitySCR_CanInstall(&authenticationWillBeRequired))
  {
    // Always do global install for Leopard.
    printf("Attempting to install Smart Crash Reporter.\n");
    OSStatus ret = UnsanitySCR_Install(kUnsanitySCR_GlobalInstall);
    if (ret != kUnsanitySCR_Install_NoError)
      printf("ERROR installing Smart Crash Reporter: %d\n", ret);
  }
  else
  {
    printf("Cannot install Smart Crash Reporter.\n");
  }
  return 0;
}