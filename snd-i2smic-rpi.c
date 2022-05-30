 /*  Filename:  snd-i2smic-rpi
     Description: module de microphone I2S
     Créé le  03/02/2022
     par Killian Vasse
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/platform_device.h>
#include <sound/simple_card.h>
#include <linux/delay.h>
#include "snd-i2smic-rpi.h"


static struct asoc_simple_card_info card_info;
static struct platform_device card_device;


static short rpi_generation;
module_param(rpi_generation, short, 0);
MODULE_PARM_DESC(rpi_generation, "Raspberry Pi generation: 0=Pi0, 1=Pi2/3, 2=Pi4");

/*
 * Setup de la carte
 */
static struct asoc_simple_card_info default_card_info = {
  .card = "snd_rpi_i2s_card",       // -> snd_soc_card.name
  .name = "simple-card_codec_link", // -> snd_soc_dai_link.name
  .codec = "snd-soc-dummy",         // "dmic-codec", // -> snd_soc_dai_link.codec_name
  .platform = "not-set.i2s",
  .daifmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM,
  .cpu_dai = {
    .name = "not-set.i2s",          // -> snd_soc_dai_link.cpu_dai_name
    .sysclk = 0
  },
  .codec_dai = {
    .name = "snd-soc-dummy-dai",    //"dmic-codec", // -> snd_soc_dai_link.codec_dai_name
    .sysclk = 0
  },
};

/*
 * Setup the card device
 */
static struct platform_device default_card_device = {
  .name = "asoc-simple-card",   //module alias
  .id = 0,
  .num_resources = 0,
  .dev = {
    .release = &device_release_callback,
    .platform_data = &default_card_info, // *HACK ALERT*
  },
};

/*
 * Callback for module initialization
 */
int i2s_mic_rpi_init(void)
{
  const char *dmaengine = "bcm2708-dmaengine"; //module name
  static char *plateformePi;
  int ret;

  printk(KERN_INFO "snd-i2smic-rpi: Version %s\n", SND_I2SMIC_RPI_VERSION);

  // Set platform
  switch (rpi_generation) {
    case 0:
      // Pi Zero
      plateformePi = "20203000.i2s";
      break;
    case 1:
      // Pi 2 and 3
      plateformePi = "3f203000.i2s";
      break;
    case 2:
    default:
      // Pi 4
      plateformePi = "fe203000.i2s";
      break;
  }

  printk(KERN_INFO "snd-i2smic-rpi: Setting platform to %s\n", plateformePi);

  // request DMA engine module
  ret = request_module(dmaengine);
  pr_alert("request module load '%s': %d\n",dmaengine, ret);

  // update info
  card_info = default_card_info;
  card_info.platform = plateformePi;
  card_info.cpu_dai.name = plateformePi;

  card_device = default_card_device;
  card_device.dev.platform_data = &card_info;

  // register the card device
  ret = platform_device_register(&card_device);
  pr_alert("register platform device '%s': %d\n",card_device.name, ret);

  return 0;
}

/*
 * Callback for module exit
 */
void i2s_mic_rpi_exit(void)
{
  platform_device_unregister(&card_device);
  pr_alert("i2s mic module unloaded\n");
}

// Plumb it up
module_init(i2s_mic_rpi_init);
module_exit(i2s_mic_rpi_exit);
MODULE_DESCRIPTION("ASoC simple-card I2S Microphone");
MODULE_AUTHOR("Carter Nelson");
MODULE_LICENSE("GPL v2");
